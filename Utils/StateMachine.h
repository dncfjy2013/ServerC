#pragma once
#include <unordered_map>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <list>
#include <atomic>
#include <functional>
#include <optional>
#include <condition_variable>
#include <shared_mutex>

template<typename TKey, typename TState>
class StateMachine {
private:
    struct StateContext {
        TState currentState;
        std::list<std::tuple<TState, std::chrono::system_clock::time_point, std::string>> history;
        std::chrono::system_clock::time_point lastUpdated;
        std::optional<std::chrono::milliseconds> timeout;
        std::optional<TState> fallbackState;
    };

    struct TimeoutTask {
        TKey key;
        std::chrono::system_clock::time_point expireTime;
        bool operator<(const TimeoutTask& other) const {
            return expireTime > other.expireTime;
        }
    };

    struct AuditLogEntry {
        std::chrono::system_clock::time_point timestamp;
        TKey key;
        TState fromState;
        TState toState;
        bool success;
        std::optional<std::string> error;
    };

    std::unordered_map<TKey, StateContext> _states;
    std::unordered_map<TKey, std::shared_mutex> _keyLocks;
    std::unordered_map<TState, std::unordered_set<TState>> _transitions;
    std::queue<AuditLogEntry> _auditLog;
    std::mutex _auditLogMutex;
    std::priority_queue<TimeoutTask> _timeoutQueue;
    std::mutex _timeoutQueueMutex;
    std::thread _timeoutScanner;
    std::atomic<bool> _stopScanner{ false };

    // 性能计数器
    std::atomic<uint64_t> _totalTransitions{ 0 };
    std::atomic<uint64_t> _successfulTransitions{ 0 };
    std::atomic<uint64_t> _failedTransitions{ 0 };

public:
    using TransitionEventHandler = std::function<void(const TKey&, const TState&, const TState&)>;
    using TransitionFailedHandler = std::function<void(const TKey&, const TState&, const TState&, const std::exception&)>;

    StateMachine() {
        _timeoutScanner = std::thread(&StateMachine::CheckTimeouts, this);
    }

    ~StateMachine() {
        _stopScanner.store(true);
        if (_timeoutScanner.joinable()) {
            _timeoutScanner.join();
        }
    }

    void InitializeState(const TKey& key, const TState& initialState) {
        std::lock_guard lock(GetKeyLock(key));
        _states.try_emplace(key, StateContext{
            .currentState = initialState,
            .lastUpdated = std::chrono::system_clock::now()
            });
    }

    void AddTransition(const TState& from, const TState& to) {
        _transitions[from].insert(to);
    }

    template<typename TransitionAction>
    bool Transition(const TKey& key, const TState& toState,
        TransitionAction&& transitionAction,
        const std::string& reason = "") {
        auto startTime = std::chrono::high_resolution_clock::now();
        _totalTransitions++;

        try {
            std::shared_lock lock(GetKeyLock(key));
            auto it = _states.find(key);
            if (it == _states.end()) return false;

            StateContext& context = it->second;
            const TState originalState = context.currentState;

            if (!_transitions.contains(originalState) ||
                !_transitions[originalState].contains(toState)) {
                return false;
            }

            lock.unlock();

            // 执行前置回调
            if (_onBeforeTransition) _onBeforeTransition(key, originalState, toState);

            // 执行转换动作
            std::optional<std::exception> actionError;
            try {
                transitionAction(key, originalState, toState);
            }
            catch (const std::exception& ex) {
                actionError = ex;
            }

            // 双重检查锁定
            {
                std::unique_lock ulock(GetKeyLock(key));
                if (it->second.currentState != originalState) return false;

                if (actionError) {
                    throw std::runtime_error("Transition failed", *actionError);
                }

                // 更新状态
                RecordHistory(context, toState, reason);
                context.currentState = toState;
                context.lastUpdated = std::chrono::system_clock::now();

                // 处理超时
                if (context.timeout.has_value()) {
                    ScheduleTimeout(key, context.timeout.value());
                }

                // 记录审计日志
                RecordAudit(key, originalState, toState, true, {});

                // 执行后置回调
                if (_onAfterTransition) _onAfterTransition(key, originalState, toState);
            }

            _successfulTransitions++;
            return true;
        }
        catch (const std::exception& ex) {
            _failedTransitions++;
            RecordAudit(key, {}, toState, false, ex.what());
            if (_onTransitionFailed) _onTransitionFailed(key, {}, toState, ex);
            return false;
        }
    }

    void SetTimeout(const TKey& key, std::chrono::milliseconds timeout, const TState& fallbackState) {
        std::unique_lock lock(GetKeyLock(key));
        auto it = _states.find(key);
        if (it == _states.end()) throw std::out_of_range("Key not found");

        it->second.timeout = timeout;
        it->second.fallbackState = fallbackState;
        ScheduleTimeout(key, timeout);
    }

    std::list<std::tuple<TState, std::chrono::system_clock::time_point, std::string>> GetStateHistory(const TKey& key) {
        std::shared_lock lock(GetKeyLock(key));
        if (auto it = _states.find(key); it != _states.end()) {
            return it->second.history;
        }
        return {};
    }

    std::list<AuditLogEntry> GetAuditLogs() {
        std::lock_guard lock(_auditLogMutex);
        return { _auditLog.begin(), _auditLog.end() };
    }

    bool TryGetCurrentState(const TKey& key, TState& state) {
        std::shared_lock lock(GetKeyLock(key));
        if (auto it = _states.find(key); it != _states.end()) {
            state = it->second.currentState;
            return true;
        }
        return false;
    }

    // 事件回调
    TransitionEventHandler _onBeforeTransition;
    TransitionEventHandler _onAfterTransition;
    TransitionFailedHandler _onTransitionFailed;

private:
    std::shared_mutex& GetKeyLock(const TKey& key) {
        static std::shared_mutex globalLock;
        auto it = _keyLocks.find(key);
        if (it != _keyLocks.end()) return it->second;
        return globalLock;
    }

    void RecordHistory(StateContext& context, const TState& newState, const std::string& reason) {
        context.history.emplace_back(newState, std::chrono::system_clock::now(), reason);
        while (context.history.size() > 100) {
            context.history.pop_front();
        }
    }

    void ScheduleTimeout(const TKey& key, std::chrono::milliseconds duration) {
        std::lock_guard lock(_timeoutQueueMutex);
        auto expireTime = std::chrono::system_clock::now() + duration;
        _timeoutQueue.emplace(TimeoutTask{ key, expireTime });
    }

    void CheckTimeouts() {
        while (!_stopScanner.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto now = std::chrono::system_clock::now();

            std::vector<TKey> expiredKeys;
            {
                std::lock_guard lock(_timeoutQueueMutex);
                while (!_timeoutQueue.empty() && _timeoutQueue.top().expireTime <= now) {
                    expiredKeys.push_back(_timeoutQueue.top().key);
                    _timeoutQueue.pop();
                }
            }

            for (const auto& key : expiredKeys) {
                HandleTimeout(key);
            }
        }
    }

    void HandleTimeout(const TKey& key) {
        std::unique_lock lock(GetKeyLock(key));
        auto it = _states.find(key);
        if (it == _states.end()) return;

        StateContext& context = it->second;
        if (!context.timeout.has_value() || !context.fallbackState.has_value()) return;

        auto elapsed = std::chrono::system_clock::now() - context.lastUpdated;
        if (elapsed < context.timeout.value()) return;

        Transition(key, context.fallbackState.value(),
            [](auto&&...) {},
            "State timeout");
    }

    void RecordAudit(const TKey& key, const TState& from, const TState& to,
        bool success, const std::optional<std::string>& error) {
        std::lock_guard lock(_auditLogMutex);
        _auditLog.push({
            std::chrono::system_clock::now(),
            key,
            from,
            to,
            success,
            error
            });

        if (_auditLog.size() > 10000) {
            _auditLog.pop();
        }
    }
};
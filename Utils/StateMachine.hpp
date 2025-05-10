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

/**
 * 通用状态机模板类，支持键值对管理多个状态实例，线程安全
 * @tparam TKey 状态机实例的键类型
 * @tparam TState 状态类型，需支持比较操作
 */
template<typename TKey, typename TState>
class StateMachine {
private:
    /**
     * 单个状态机实例的上下文信息
     */
    struct StateContext {
        TState currentState;                      // 当前状态
        std::list<std::tuple<TState, std::chrono::system_clock::time_point, std::string>> history; // 状态变更历史
        std::chrono::system_clock::time_point lastUpdated; // 最后更新时间
        std::optional<std::chrono::milliseconds> timeout; // 状态超时时间
        std::optional<TState> fallbackState;     // 超时后的回退状态
    };

    /**
     * 超时任务结构，用于优先级队列
     */
    struct TimeoutTask {
        TKey key;                              // 关联的状态机键
        std::chrono::system_clock::time_point expireTime; // 过期时间点
        // 重载比较运算符，使优先级队列按过期时间排序
        bool operator<(const TimeoutTask& other) const {
            return expireTime > other.expireTime; // 小顶堆排序
        }
    };

    /**
     * 审计日志条目结构
     */
    struct AuditLogEntry {
        std::chrono::system_clock::time_point timestamp; // 时间戳
        TKey key;                              // 状态机键
        TState fromState;                      // 源状态
        TState toState;                        // 目标状态
        bool success;                          // 转换是否成功
        std::optional<std::string> error;      // 错误信息（如果有）
    };

    // 核心数据结构
    std::unordered_map<TKey, StateContext> _states; // 状态机实例映射
    std::unordered_map<TKey, std::shared_mutex> _keyLocks; // 每个键的读写锁
    std::unordered_map<TState, std::unordered_set<TState>> _transitions; // 合法状态转换表
    std::queue<AuditLogEntry> _auditLog; // 审计日志队列
    std::mutex _auditLogMutex; // 审计日志互斥锁
    std::priority_queue<TimeoutTask> _timeoutQueue; // 超时任务优先级队列
    std::mutex _timeoutQueueMutex; // 超时队列互斥锁
    std::thread _timeoutScanner; // 超时扫描线程
    std::atomic<bool> _stopScanner{ false }; // 扫描线程停止标志

    // 性能计数器
    std::atomic<uint64_t> _totalTransitions{ 0 }; // 总状态转换次数
    std::atomic<uint64_t> _successfulTransitions{ 0 }; // 成功转换次数
    std::atomic<uint64_t> _failedTransitions{ 0 }; // 失败转换次数

public:
    // 状态转换事件处理函数类型定义
    using TransitionEventHandler = std::function<void(const TKey&, const TState&, const TState&)>;
    // 状态转换失败处理函数类型定义
    using TransitionFailedHandler = std::function<void(const TKey&, const TState&, const TState&, const std::exception&)>;

    /**
     * 构造函数，初始化超时扫描线程
     */
    StateMachine() {
        _timeoutScanner = std::thread(&StateMachine::CheckTimeouts, this);
    }

    /**
     * 析构函数，停止超时扫描线程并等待其结束
     */
    ~StateMachine() {
        _stopScanner.store(true);
        if (_timeoutScanner.joinable()) {
            _timeoutScanner.join();
        }
    }

    /**
     * 初始化指定键的状态机实例
     * @param key 状态机键
     * @param initialState 初始状态
     */
    void InitializeState(const TKey& key, const TState& initialState) {
        std::lock_guard lock(GetKeyLock(key));
        _states.try_emplace(key, StateContext{
            .currentState = initialState,
            .lastUpdated = std::chrono::system_clock::now()
            });
    }

    /**
     * 添加合法的状态转换规则
     * @param from 源状态
     * @param to 目标状态
     */
    void AddTransition(const TState& from, const TState& to) {
        _transitions[from].insert(to);
    }

    /**
     * 执行状态转换
     * @tparam TransitionAction 转换动作函数类型
     * @param key 状态机键
     * @param toState 目标状态
     * @param transitionAction 状态转换时执行的动作
     * @param reason 状态转换原因（用于记录历史）
     * @return 转换是否成功
     */
    template<typename TransitionAction>
    bool Transition(const TKey& key, const TState& toState,
        TransitionAction&& transitionAction,
        const std::string& reason = "") {
        auto startTime = std::chrono::high_resolution_clock::now();
        _totalTransitions++;

        try {
            // 先获取共享锁检查状态
            std::shared_lock lock(GetKeyLock(key));
            auto it = _states.find(key);
            if (it == _states.end()) return false;

            StateContext& context = it->second;
            const TState originalState = context.currentState;

            // 检查是否为合法转换
            if (!_transitions.contains(originalState) ||
                !_transitions[originalState].contains(toState)) {
                return false;
            }

            lock.unlock();

            // 执行前置回调（如果有）
            if (_onBeforeTransition) _onBeforeTransition(key, originalState, toState);

            // 执行转换动作，捕获可能的异常
            std::optional<std::exception> actionError;
            try {
                transitionAction(key, originalState, toState);
            }
            catch (const std::exception& ex) {
                actionError = ex;
            }

            // 双重检查锁定，获取排他锁进行状态更新
            {
                std::unique_lock ulock(GetKeyLock(key));
                // 检查状态是否被其他线程修改
                if (it->second.currentState != originalState) return false;

                // 如果转换动作抛出异常，记录错误并终止转换
                if (actionError) {
                    throw std::runtime_error("Transition failed", *actionError);
                }

                // 记录状态变更历史
                RecordHistory(context, toState, reason);
                // 更新当前状态
                context.currentState = toState;
                // 更新最后更新时间
                context.lastUpdated = std::chrono::system_clock::now();

                // 如果设置了超时，安排超时任务
                if (context.timeout.has_value()) {
                    ScheduleTimeout(key, context.timeout.value());
                }

                // 记录审计日志
                RecordAudit(key, originalState, toState, true, {});

                // 执行后置回调（如果有）
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

    /**
     * 设置状态超时
     * @param key 状态机键
     * @param timeout 超时时间
     * @param fallbackState 超时后自动转换的回退状态
     */
    void SetTimeout(const TKey& key, std::chrono::milliseconds timeout, const TState& fallbackState) {
        std::unique_lock lock(GetKeyLock(key));
        auto it = _states.find(key);
        if (it == _states.end()) throw std::out_of_range("Key not found");

        it->second.timeout = timeout;
        it->second.fallbackState = fallbackState;
        ScheduleTimeout(key, timeout);
    }

    /**
     * 获取指定键的状态变更历史
     * @param key 状态机键
     * @return 状态变更历史列表
     */
    std::list<std::tuple<TState, std::chrono::system_clock::time_point, std::string>> GetStateHistory(const TKey& key) {
        std::shared_lock lock(GetKeyLock(key));
        if (auto it = _states.find(key); it != _states.end()) {
            return it->second.history;
        }
        return {};
    }

    /**
     * 获取审计日志
     * @return 审计日志列表
     */
    std::list<AuditLogEntry> GetAuditLogs() {
        std::lock_guard lock(_auditLogMutex);
        return { _auditLog.begin(), _auditLog.end() };
    }

    /**
     * 尝试获取当前状态
     * @param key 状态机键
     * @param state 用于存储状态的引用
     * @return 是否成功获取状态
     */
    bool TryGetCurrentState(const TKey& key, TState& state) {
        std::shared_lock lock(GetKeyLock(key));
        if (auto it = _states.find(key); it != _states.end()) {
            state = it->second.currentState;
            return true;
        }
        return false;
    }

    // 事件回调函数
    TransitionEventHandler _onBeforeTransition; // 状态转换前回调
    TransitionEventHandler _onAfterTransition;  // 状态转换后回调
    TransitionFailedHandler _onTransitionFailed; // 状态转换失败回调

private:
    /**
     * 获取指定键的读写锁（如果不存在则返回全局锁）
     * @param key 状态机键
     * @return 读写锁引用
     */
    std::shared_mutex& GetKeyLock(const TKey& key) {
        static std::shared_mutex globalLock;
        auto it = _keyLocks.find(key);
        if (it != _keyLocks.end()) return it->second;
        return globalLock;
    }

    /**
     * 记录状态变更历史
     * @param context 状态上下文
     * @param newState 新状态
     * @param reason 变更原因
     */
    void RecordHistory(StateContext& context, const TState& newState, const std::string& reason) {
        context.history.emplace_back(newState, std::chrono::system_clock::now(), reason);
        // 限制历史记录数量，防止内存溢出
        while (context.history.size() > 100) {
            context.history.pop_front();
        }
    }

    /**
     * 安排超时任务
     * @param key 状态机键
     * @param duration 超时时间
     */
    void ScheduleTimeout(const TKey& key, std::chrono::milliseconds duration) {
        std::lock_guard lock(_timeoutQueueMutex);
        auto expireTime = std::chrono::system_clock::now() + duration;
        _timeoutQueue.emplace(TimeoutTask{ key, expireTime });
    }

    /**
     * 超时扫描线程主函数，定期检查超时任务
     */
    void CheckTimeouts() {
        while (!_stopScanner.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 每秒检查一次
            auto now = std::chrono::system_clock::now();

            // 收集所有已过期的键
            std::vector<TKey> expiredKeys;
            {
                std::lock_guard lock(_timeoutQueueMutex);
                while (!_timeoutQueue.empty() && _timeoutQueue.top().expireTime <= now) {
                    expiredKeys.push_back(_timeoutQueue.top().key);
                    _timeoutQueue.pop();
                }
            }

            // 处理所有过期的状态机
            for (const auto& key : expiredKeys) {
                HandleTimeout(key);
            }
        }
    }

    /**
     * 处理超时的状态机
     * @param key 状态机键
     */
    void HandleTimeout(const TKey& key) {
        std::unique_lock lock(GetKeyLock(key));
        auto it = _states.find(key);
        if (it == _states.end()) return;

        StateContext& context = it->second;
        // 检查是否设置了超时和回退状态
        if (!context.timeout.has_value() || !context.fallbackState.has_value()) return;

        // 检查是否真的超时
        auto elapsed = std::chrono::system_clock::now() - context.lastUpdated;
        if (elapsed < context.timeout.value()) return;

        // 执行超时状态转换
        Transition(key, context.fallbackState.value(),
            [](auto&&...) {},
            "State timeout");
    }

    /**
     * 记录审计日志
     * @param key 状态机键
     * @param from 源状态
     * @param to 目标状态
     * @param success 是否成功
     * @param error 错误信息（如果有）
     */
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

        // 限制审计日志数量，防止内存溢出
        if (_auditLog.size() > 10000) {
            _auditLog.pop();
        }
    }
};
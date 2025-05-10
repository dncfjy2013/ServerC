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
 * ͨ��״̬��ģ���֧࣬�ּ�ֵ�Թ�����״̬ʵ�����̰߳�ȫ
 * @tparam TKey ״̬��ʵ���ļ�����
 * @tparam TState ״̬���ͣ���֧�ֱȽϲ���
 */
template<typename TKey, typename TState>
class StateMachine {
private:
    /**
     * ����״̬��ʵ������������Ϣ
     */
    struct StateContext {
        TState currentState;                      // ��ǰ״̬
        std::list<std::tuple<TState, std::chrono::system_clock::time_point, std::string>> history; // ״̬�����ʷ
        std::chrono::system_clock::time_point lastUpdated; // ������ʱ��
        std::optional<std::chrono::milliseconds> timeout; // ״̬��ʱʱ��
        std::optional<TState> fallbackState;     // ��ʱ��Ļ���״̬
    };

    /**
     * ��ʱ����ṹ���������ȼ�����
     */
    struct TimeoutTask {
        TKey key;                              // ������״̬����
        std::chrono::system_clock::time_point expireTime; // ����ʱ���
        // ���رȽ��������ʹ���ȼ����а�����ʱ������
        bool operator<(const TimeoutTask& other) const {
            return expireTime > other.expireTime; // С��������
        }
    };

    /**
     * �����־��Ŀ�ṹ
     */
    struct AuditLogEntry {
        std::chrono::system_clock::time_point timestamp; // ʱ���
        TKey key;                              // ״̬����
        TState fromState;                      // Դ״̬
        TState toState;                        // Ŀ��״̬
        bool success;                          // ת���Ƿ�ɹ�
        std::optional<std::string> error;      // ������Ϣ������У�
    };

    // �������ݽṹ
    std::unordered_map<TKey, StateContext> _states; // ״̬��ʵ��ӳ��
    std::unordered_map<TKey, std::shared_mutex> _keyLocks; // ÿ�����Ķ�д��
    std::unordered_map<TState, std::unordered_set<TState>> _transitions; // �Ϸ�״̬ת����
    std::queue<AuditLogEntry> _auditLog; // �����־����
    std::mutex _auditLogMutex; // �����־������
    std::priority_queue<TimeoutTask> _timeoutQueue; // ��ʱ�������ȼ�����
    std::mutex _timeoutQueueMutex; // ��ʱ���л�����
    std::thread _timeoutScanner; // ��ʱɨ���߳�
    std::atomic<bool> _stopScanner{ false }; // ɨ���߳�ֹͣ��־

    // ���ܼ�����
    std::atomic<uint64_t> _totalTransitions{ 0 }; // ��״̬ת������
    std::atomic<uint64_t> _successfulTransitions{ 0 }; // �ɹ�ת������
    std::atomic<uint64_t> _failedTransitions{ 0 }; // ʧ��ת������

public:
    // ״̬ת���¼����������Ͷ���
    using TransitionEventHandler = std::function<void(const TKey&, const TState&, const TState&)>;
    // ״̬ת��ʧ�ܴ��������Ͷ���
    using TransitionFailedHandler = std::function<void(const TKey&, const TState&, const TState&, const std::exception&)>;

    /**
     * ���캯������ʼ����ʱɨ���߳�
     */
    StateMachine() {
        _timeoutScanner = std::thread(&StateMachine::CheckTimeouts, this);
    }

    /**
     * ����������ֹͣ��ʱɨ���̲߳��ȴ������
     */
    ~StateMachine() {
        _stopScanner.store(true);
        if (_timeoutScanner.joinable()) {
            _timeoutScanner.join();
        }
    }

    /**
     * ��ʼ��ָ������״̬��ʵ��
     * @param key ״̬����
     * @param initialState ��ʼ״̬
     */
    void InitializeState(const TKey& key, const TState& initialState) {
        std::lock_guard lock(GetKeyLock(key));
        _states.try_emplace(key, StateContext{
            .currentState = initialState,
            .lastUpdated = std::chrono::system_clock::now()
            });
    }

    /**
     * ��ӺϷ���״̬ת������
     * @param from Դ״̬
     * @param to Ŀ��״̬
     */
    void AddTransition(const TState& from, const TState& to) {
        _transitions[from].insert(to);
    }

    /**
     * ִ��״̬ת��
     * @tparam TransitionAction ת��������������
     * @param key ״̬����
     * @param toState Ŀ��״̬
     * @param transitionAction ״̬ת��ʱִ�еĶ���
     * @param reason ״̬ת��ԭ�����ڼ�¼��ʷ��
     * @return ת���Ƿ�ɹ�
     */
    template<typename TransitionAction>
    bool Transition(const TKey& key, const TState& toState,
        TransitionAction&& transitionAction,
        const std::string& reason = "") {
        auto startTime = std::chrono::high_resolution_clock::now();
        _totalTransitions++;

        try {
            // �Ȼ�ȡ���������״̬
            std::shared_lock lock(GetKeyLock(key));
            auto it = _states.find(key);
            if (it == _states.end()) return false;

            StateContext& context = it->second;
            const TState originalState = context.currentState;

            // ����Ƿ�Ϊ�Ϸ�ת��
            if (!_transitions.contains(originalState) ||
                !_transitions[originalState].contains(toState)) {
                return false;
            }

            lock.unlock();

            // ִ��ǰ�ûص�������У�
            if (_onBeforeTransition) _onBeforeTransition(key, originalState, toState);

            // ִ��ת��������������ܵ��쳣
            std::optional<std::exception> actionError;
            try {
                transitionAction(key, originalState, toState);
            }
            catch (const std::exception& ex) {
                actionError = ex;
            }

            // ˫�ؼ����������ȡ����������״̬����
            {
                std::unique_lock ulock(GetKeyLock(key));
                // ���״̬�Ƿ������߳��޸�
                if (it->second.currentState != originalState) return false;

                // ���ת�������׳��쳣����¼������ֹת��
                if (actionError) {
                    throw std::runtime_error("Transition failed", *actionError);
                }

                // ��¼״̬�����ʷ
                RecordHistory(context, toState, reason);
                // ���µ�ǰ״̬
                context.currentState = toState;
                // ����������ʱ��
                context.lastUpdated = std::chrono::system_clock::now();

                // ��������˳�ʱ�����ų�ʱ����
                if (context.timeout.has_value()) {
                    ScheduleTimeout(key, context.timeout.value());
                }

                // ��¼�����־
                RecordAudit(key, originalState, toState, true, {});

                // ִ�к��ûص�������У�
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
     * ����״̬��ʱ
     * @param key ״̬����
     * @param timeout ��ʱʱ��
     * @param fallbackState ��ʱ���Զ�ת���Ļ���״̬
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
     * ��ȡָ������״̬�����ʷ
     * @param key ״̬����
     * @return ״̬�����ʷ�б�
     */
    std::list<std::tuple<TState, std::chrono::system_clock::time_point, std::string>> GetStateHistory(const TKey& key) {
        std::shared_lock lock(GetKeyLock(key));
        if (auto it = _states.find(key); it != _states.end()) {
            return it->second.history;
        }
        return {};
    }

    /**
     * ��ȡ�����־
     * @return �����־�б�
     */
    std::list<AuditLogEntry> GetAuditLogs() {
        std::lock_guard lock(_auditLogMutex);
        return { _auditLog.begin(), _auditLog.end() };
    }

    /**
     * ���Ի�ȡ��ǰ״̬
     * @param key ״̬����
     * @param state ���ڴ洢״̬������
     * @return �Ƿ�ɹ���ȡ״̬
     */
    bool TryGetCurrentState(const TKey& key, TState& state) {
        std::shared_lock lock(GetKeyLock(key));
        if (auto it = _states.find(key); it != _states.end()) {
            state = it->second.currentState;
            return true;
        }
        return false;
    }

    // �¼��ص�����
    TransitionEventHandler _onBeforeTransition; // ״̬ת��ǰ�ص�
    TransitionEventHandler _onAfterTransition;  // ״̬ת����ص�
    TransitionFailedHandler _onTransitionFailed; // ״̬ת��ʧ�ܻص�

private:
    /**
     * ��ȡָ�����Ķ�д��������������򷵻�ȫ������
     * @param key ״̬����
     * @return ��д������
     */
    std::shared_mutex& GetKeyLock(const TKey& key) {
        static std::shared_mutex globalLock;
        auto it = _keyLocks.find(key);
        if (it != _keyLocks.end()) return it->second;
        return globalLock;
    }

    /**
     * ��¼״̬�����ʷ
     * @param context ״̬������
     * @param newState ��״̬
     * @param reason ���ԭ��
     */
    void RecordHistory(StateContext& context, const TState& newState, const std::string& reason) {
        context.history.emplace_back(newState, std::chrono::system_clock::now(), reason);
        // ������ʷ��¼��������ֹ�ڴ����
        while (context.history.size() > 100) {
            context.history.pop_front();
        }
    }

    /**
     * ���ų�ʱ����
     * @param key ״̬����
     * @param duration ��ʱʱ��
     */
    void ScheduleTimeout(const TKey& key, std::chrono::milliseconds duration) {
        std::lock_guard lock(_timeoutQueueMutex);
        auto expireTime = std::chrono::system_clock::now() + duration;
        _timeoutQueue.emplace(TimeoutTask{ key, expireTime });
    }

    /**
     * ��ʱɨ���߳������������ڼ�鳬ʱ����
     */
    void CheckTimeouts() {
        while (!_stopScanner.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // ÿ����һ��
            auto now = std::chrono::system_clock::now();

            // �ռ������ѹ��ڵļ�
            std::vector<TKey> expiredKeys;
            {
                std::lock_guard lock(_timeoutQueueMutex);
                while (!_timeoutQueue.empty() && _timeoutQueue.top().expireTime <= now) {
                    expiredKeys.push_back(_timeoutQueue.top().key);
                    _timeoutQueue.pop();
                }
            }

            // �������й��ڵ�״̬��
            for (const auto& key : expiredKeys) {
                HandleTimeout(key);
            }
        }
    }

    /**
     * ����ʱ��״̬��
     * @param key ״̬����
     */
    void HandleTimeout(const TKey& key) {
        std::unique_lock lock(GetKeyLock(key));
        auto it = _states.find(key);
        if (it == _states.end()) return;

        StateContext& context = it->second;
        // ����Ƿ������˳�ʱ�ͻ���״̬
        if (!context.timeout.has_value() || !context.fallbackState.has_value()) return;

        // ����Ƿ���ĳ�ʱ
        auto elapsed = std::chrono::system_clock::now() - context.lastUpdated;
        if (elapsed < context.timeout.value()) return;

        // ִ�г�ʱ״̬ת��
        Transition(key, context.fallbackState.value(),
            [](auto&&...) {},
            "State timeout");
    }

    /**
     * ��¼�����־
     * @param key ״̬����
     * @param from Դ״̬
     * @param to Ŀ��״̬
     * @param success �Ƿ�ɹ�
     * @param error ������Ϣ������У�
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

        // ���������־��������ֹ�ڴ����
        if (_auditLog.size() > 10000) {
            _auditLog.pop();
        }
    }
};
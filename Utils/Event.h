#pragma once
#include <functional>
#include <vector>
#include <mutex>

template<typename... Args>
class ThreadSafeEvent {
    std::vector<std::function<void(Args...)>> handlers;
    mutable std::mutex mtx;
public:
    void operator+=(const std::function<void(Args...)>& handler) {
        std::lock_guard lock(mtx);
        handlers.push_back(handler);
    }

    void operator()(Args... args) const {
        std::lock_guard lock(mtx);
        for (auto& h : handlers) {
            h(args...);
        }
    }
};
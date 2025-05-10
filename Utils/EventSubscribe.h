#pragma once
#include <functional>
#include <vector>
#include <mutex>
#include <algorithm>
#include <memory>

/**
 * 线程安全的事件订阅发布系统
 * 支持任意参数类型的事件，使用智能指针作为订阅令牌
 * 提供线程安全的订阅、取消订阅和事件发布操作
 */
template<typename... Args>
class SubscribeEvent {
    // 内部订阅实现结构体
    struct SubscriptionImpl {
        std::function<void(Args...)> handler; // 事件处理函数
        std::shared_ptr<void> token;          // 唯一标识订阅的智能指针
    };

    mutable std::mutex mtx;                   // 保护订阅列表的互斥锁
    std::vector<SubscriptionImpl> subscriptions; // 订阅列表

public:
    using Subscription = std::shared_ptr<void>;  // 订阅令牌类型定义

    /**
     * 订阅事件处理函数
     * @param handler 事件触发时调用的处理函数
     * @return 返回唯一的订阅令牌，用于取消订阅
     *
     * 线程安全操作，使用互斥锁保护订阅列表
     */
    Subscription subscribe(const std::function<void(Args...)>& handler) {
        std::lock_guard lock(mtx);
        // 创建唯一的共享指针作为订阅令牌
        auto token = std::make_shared<void>();
        // 将处理函数和令牌添加到订阅列表
        subscriptions.push_back({ handler, token });
        return token;
    }

    /**
     * 取消订阅
     * @param token 订阅时返回的令牌
     *
     * 线程安全操作，使用互斥锁保护订阅列表
     */
    void unsubscribe(const Subscription& token) {
        std::lock_guard lock(mtx);
        // 使用erase-remove惯用法移除匹配的订阅
        subscriptions.erase(
            std::remove_if(subscriptions.begin(), subscriptions.end(),
                [&](const SubscriptionImpl& sub) {
                    return sub.token == token;
                }),
            subscriptions.end());
    }

    /**
     * 发布事件（触发所有订阅的处理函数）
     * @param args 传递给处理函数的参数，支持完美转发
     *
     * 线程安全操作，使用互斥锁保护订阅列表
     * 异常安全：单个处理函数的异常不会影响其他处理函数
     */
    void operator()(Args&&... args) const {
        std::lock_guard lock(mtx);
        // 遍历所有订阅并调用处理函数
        for (auto& sub : subscriptions) {
            try {
                // 完美转发参数，保持参数的值类别（左值/右值）
                sub.handler(std::forward<Args>(args)...);
            }
            catch (const std::exception& e) {
                // 处理已知异常，可根据需求扩展
            }
            catch (...) {
                // 处理未知异常，确保一个处理函数的异常不会影响其他处理函数
            }
        }
    }
};

//// 创建事件对象（接受int和string参数）
//SubscribeEvent<int, std::string> logger;
//
//// 订阅事件（返回订阅令牌）
//auto sub1 = logger.subscribe([](int code, const std::string& msg) {
//    std::cout << "[Log1] Code: " << code << ", Msg: " << msg << std::endl;
//    });
//
//auto sub2 = logger.subscribe([](int code, std::string msg) {
//    std::cout << "[Log2] Code: " << code << ", Msg: " << std::move(msg) << std::endl;
//    });
//
//// 发布事件（参数会被完美转发）
//logger(200, "Request successful");
//
//// 取消第二个订阅
//logger.unsubscribe(sub2);
//
//// 再次发布事件（只有第一个订阅会被触发）
//logger(404, "Not Found");
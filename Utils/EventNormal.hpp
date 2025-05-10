#pragma once
#include <functional>
#include <vector>
#include <mutex>

template<typename... Args>
class ThreadSafeEventNormal {
    // 存储所有已订阅的事件处理函数
    std::vector<std::function<void(Args...)>> handlers;
    // 保护handlers访问的互斥量
    mutable std::mutex mtx;
public:
    // 订阅事件处理函数
    // 参数handler是符合签名的可调用对象，如函数、lambda表达式、函数对象等
    // 通过加锁来保证在多线程环境下添加处理函数时的线程安全性
    // 加锁可以防止多个线程同时修改handlers，避免迭代器失效等问题
    void operator+=(const std::function<void(Args...)>& handler) {
        std::lock_guard<std::mutex> lock(mtx);
        handlers.push_back(handler);
    }

    // 发布事件，触发所有订阅的处理函数
    // 参数args是传递给处理函数的参数，这里按值传递（可能产生拷贝）
    // 通过加锁来保证在多线程环境下访问handlers的线程安全性
    // 先创建handlers的本地副本，解锁后执行处理函数，减少锁的持有时间
    // 如果处理函数中抛出异常，后续处理函数将不会执行
    void operator()(Args... args) const {
        std::lock_guard<std::mutex> lock(mtx);

        // 创建临时副本保证执行期间handlers不被其他线程修改
        auto localHandlers = handlers;

        // 解锁，避免在执行处理函数时长时间持有锁
        lock.unlock();

        for (const auto& h : localHandlers) {
            h(args...);
        }
    }

    // 取消订阅函数，用于从已订阅的处理函数列表中移除指定的处理函数
    // 通过加锁来保证在多线程环境下移除处理函数时的线程安全性
    // 目前只是预留接口，尚未实现具体的查找和移除逻辑，直接返回true
    bool operator-=(const std::function<void(Args...)>& handler) {
        std::lock_guard<std::mutex> lock(mtx);
        // 这里应实现具体的查找和移除逻辑，当前只是占位
        return true;
    }

    // 异步发布事件，使用指定的执行器exec来异步执行已订阅的处理函数
    // 参数exec是一个执行器，用于异步执行处理函数
    // 参数args是传递给处理函数的参数，按值传递
    // 通过加锁来保证在多线程环境下访问handlers的线程安全性
    // 先创建handlers的本地副本，解锁后使用执行器异步执行处理函数
    template<typename Executor>
    void Async(Executor&& exec, Args... args) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto localHandlers = handlers;
        lock.unlock();

        for (const auto& h : localHandlers) {
            exec([=] { h(args...); });
        }
    }
};

//// 示例1：基础用法
//void BasicUsage() {
//    ThreadSafeEventNormal<int> intEvent;
//
//    // 订阅处理函数（方式1：lambda）
//    intEvent += [](int x) {
//        std::cout << "Received (lambda): " << x << std::endl;
//        };
//
//    // 订阅处理函数（方式2：函数对象）
//    struct Printer {
//        void operator()(int val) const {
//            std::cout << "Received (functor): " << val * 2 << std::endl;
//        }
//    };
//    intEvent += Printer();
//
//    // 发布事件
//    intEvent(42);  // 输出：
//    // Received (lambda): 42
//    // Received (functor): 84
//
//// 取消订阅（需要实现operator-=）
//// intEvent -= Printer();
//}
//
//// 示例2：多线程安全验证
//void MultithreadedUsage() {
//    ThreadSafeEventNormal<std::string> logEvent;
//    const int threadCount = 5;
//    const int iterations = 100;
//
//    // 启动多个写入线程
//    auto writer = [&](int id) {
//        for (int i = 0; i < iterations; ++i) {
//            logEvent("Thread " + std::to_string(id) + " - Msg " + std::to_string(i));
//        }
//        };
//
//    std::vector<std::thread> writers;
//    for (int i = 0; i < threadCount; ++i) {
//        writers.emplace_back(writer, i);
//    }
//
//    // 订阅日志处理器（主线程）
//    logEvent += [](const std::string& msg) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 模拟IO操作
//        std::cout << "[LOG] " << msg << std::endl;
//        };
//
//    for (auto& t : writers) t.join();
//}
//
//// 示例3：异常处理演示
//void ExceptionHandling() {
//    ThreadSafeEventNormal<int> errorDemo;
//
//    errorDemo += [](int) { throw std::runtime_error("Oops!"); };
//    errorDemo += [](int x) { std::cout << "This will not execute: " << x << std::endl; };
//
//    try {
//        errorDemo(123);  // 第一个处理函数抛出异常，第二个不会执行
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Caught exception: " << e.what() << std::endl;
//    }
//}
//
//// 示例4：异步发布（需要C++17或更高版本）
//#include <execution>
//void AsyncUsage() {
//    ThreadSafeEventNormal<int> asyncEvent;
//
//    // 订阅耗时操作
//    asyncEvent += [](int x) {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        std::cout << "Async result: " << x * 2 << std::endl;
//        };
//
//    // 异步触发事件（使用标准线程池）
//    asyncEvent.Async(std::execution::par, 21);
//
//    std::cout << "Main thread continues..." << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(2));  // 等待异步操作完成
//}
//
//int main() {
//    // 运行所有示例
//    BasicUsage();
//    MultithreadedUsage();
//    ExceptionHandling();
//    AsyncUsage();
//
//    return 0;
//}
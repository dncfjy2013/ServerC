#pragma once
#include <functional>
#include <vector>
#include <mutex>

template<typename... Args>
class ThreadSafeEventNormal {
    // �洢�����Ѷ��ĵ��¼�������
    std::vector<std::function<void(Args...)>> handlers;
    // ����handlers���ʵĻ�����
    mutable std::mutex mtx;
public:
    // �����¼�������
    // ����handler�Ƿ���ǩ���Ŀɵ��ö����纯����lambda���ʽ�����������
    // ͨ����������֤�ڶ��̻߳�������Ӵ�����ʱ���̰߳�ȫ��
    // �������Է�ֹ����߳�ͬʱ�޸�handlers�����������ʧЧ������
    void operator+=(const std::function<void(Args...)>& handler) {
        std::lock_guard<std::mutex> lock(mtx);
        handlers.push_back(handler);
    }

    // �����¼����������ж��ĵĴ�����
    // ����args�Ǵ��ݸ��������Ĳ��������ﰴֵ���ݣ����ܲ���������
    // ͨ����������֤�ڶ��̻߳����·���handlers���̰߳�ȫ��
    // �ȴ���handlers�ı��ظ�����������ִ�д��������������ĳ���ʱ��
    // ������������׳��쳣������������������ִ��
    void operator()(Args... args) const {
        std::lock_guard<std::mutex> lock(mtx);

        // ������ʱ������ִ֤���ڼ�handlers���������߳��޸�
        auto localHandlers = handlers;

        // ������������ִ�д�����ʱ��ʱ�������
        lock.unlock();

        for (const auto& h : localHandlers) {
            h(args...);
        }
    }

    // ȡ�����ĺ��������ڴ��Ѷ��ĵĴ������б����Ƴ�ָ���Ĵ�����
    // ͨ����������֤�ڶ��̻߳������Ƴ�������ʱ���̰߳�ȫ��
    // Ŀǰֻ��Ԥ���ӿڣ���δʵ�־���Ĳ��Һ��Ƴ��߼���ֱ�ӷ���true
    bool operator-=(const std::function<void(Args...)>& handler) {
        std::lock_guard<std::mutex> lock(mtx);
        // ����Ӧʵ�־���Ĳ��Һ��Ƴ��߼�����ǰֻ��ռλ
        return true;
    }

    // �첽�����¼���ʹ��ָ����ִ����exec���첽ִ���Ѷ��ĵĴ�����
    // ����exec��һ��ִ�����������첽ִ�д�����
    // ����args�Ǵ��ݸ��������Ĳ�������ֵ����
    // ͨ����������֤�ڶ��̻߳����·���handlers���̰߳�ȫ��
    // �ȴ���handlers�ı��ظ�����������ʹ��ִ�����첽ִ�д�����
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

//// ʾ��1�������÷�
//void BasicUsage() {
//    ThreadSafeEventNormal<int> intEvent;
//
//    // ���Ĵ���������ʽ1��lambda��
//    intEvent += [](int x) {
//        std::cout << "Received (lambda): " << x << std::endl;
//        };
//
//    // ���Ĵ���������ʽ2����������
//    struct Printer {
//        void operator()(int val) const {
//            std::cout << "Received (functor): " << val * 2 << std::endl;
//        }
//    };
//    intEvent += Printer();
//
//    // �����¼�
//    intEvent(42);  // �����
//    // Received (lambda): 42
//    // Received (functor): 84
//
//// ȡ�����ģ���Ҫʵ��operator-=��
//// intEvent -= Printer();
//}
//
//// ʾ��2�����̰߳�ȫ��֤
//void MultithreadedUsage() {
//    ThreadSafeEventNormal<std::string> logEvent;
//    const int threadCount = 5;
//    const int iterations = 100;
//
//    // �������д���߳�
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
//    // ������־�����������̣߳�
//    logEvent += [](const std::string& msg) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // ģ��IO����
//        std::cout << "[LOG] " << msg << std::endl;
//        };
//
//    for (auto& t : writers) t.join();
//}
//
//// ʾ��3���쳣������ʾ
//void ExceptionHandling() {
//    ThreadSafeEventNormal<int> errorDemo;
//
//    errorDemo += [](int) { throw std::runtime_error("Oops!"); };
//    errorDemo += [](int x) { std::cout << "This will not execute: " << x << std::endl; };
//
//    try {
//        errorDemo(123);  // ��һ���������׳��쳣���ڶ�������ִ��
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Caught exception: " << e.what() << std::endl;
//    }
//}
//
//// ʾ��4���첽��������ҪC++17����߰汾��
//#include <execution>
//void AsyncUsage() {
//    ThreadSafeEventNormal<int> asyncEvent;
//
//    // ���ĺ�ʱ����
//    asyncEvent += [](int x) {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        std::cout << "Async result: " << x * 2 << std::endl;
//        };
//
//    // �첽�����¼���ʹ�ñ�׼�̳߳أ�
//    asyncEvent.Async(std::execution::par, 21);
//
//    std::cout << "Main thread continues..." << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(2));  // �ȴ��첽�������
//}
//
//int main() {
//    // ��������ʾ��
//    BasicUsage();
//    MultithreadedUsage();
//    ExceptionHandling();
//    AsyncUsage();
//
//    return 0;
//}
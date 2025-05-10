#pragma once
#include <functional>
#include <vector>
#include <mutex>
#include <algorithm>
#include <memory>

/**
 * �̰߳�ȫ���¼����ķ���ϵͳ
 * ֧������������͵��¼���ʹ������ָ����Ϊ��������
 * �ṩ�̰߳�ȫ�Ķ��ġ�ȡ�����ĺ��¼���������
 */
template<typename... Args>
class SubscribeEvent {
    // �ڲ�����ʵ�ֽṹ��
    struct SubscriptionImpl {
        std::function<void(Args...)> handler; // �¼�������
        std::shared_ptr<void> token;          // Ψһ��ʶ���ĵ�����ָ��
    };

    mutable std::mutex mtx;                   // ���������б�Ļ�����
    std::vector<SubscriptionImpl> subscriptions; // �����б�

public:
    using Subscription = std::shared_ptr<void>;  // �����������Ͷ���

    /**
     * �����¼�������
     * @param handler �¼�����ʱ���õĴ�����
     * @return ����Ψһ�Ķ������ƣ�����ȡ������
     *
     * �̰߳�ȫ������ʹ�û��������������б�
     */
    Subscription subscribe(const std::function<void(Args...)>& handler) {
        std::lock_guard lock(mtx);
        // ����Ψһ�Ĺ���ָ����Ϊ��������
        auto token = std::make_shared<void>();
        // ����������������ӵ������б�
        subscriptions.push_back({ handler, token });
        return token;
    }

    /**
     * ȡ������
     * @param token ����ʱ���ص�����
     *
     * �̰߳�ȫ������ʹ�û��������������б�
     */
    void unsubscribe(const Subscription& token) {
        std::lock_guard lock(mtx);
        // ʹ��erase-remove���÷��Ƴ�ƥ��Ķ���
        subscriptions.erase(
            std::remove_if(subscriptions.begin(), subscriptions.end(),
                [&](const SubscriptionImpl& sub) {
                    return sub.token == token;
                }),
            subscriptions.end());
    }

    /**
     * �����¼����������ж��ĵĴ�������
     * @param args ���ݸ��������Ĳ�����֧������ת��
     *
     * �̰߳�ȫ������ʹ�û��������������б�
     * �쳣��ȫ���������������쳣����Ӱ������������
     */
    void operator()(Args&&... args) const {
        std::lock_guard lock(mtx);
        // �������ж��Ĳ����ô�����
        for (auto& sub : subscriptions) {
            try {
                // ����ת�����������ֲ�����ֵ�����ֵ/��ֵ��
                sub.handler(std::forward<Args>(args)...);
            }
            catch (const std::exception& e) {
                // ������֪�쳣���ɸ���������չ
            }
            catch (...) {
                // ����δ֪�쳣��ȷ��һ�����������쳣����Ӱ������������
            }
        }
    }
};

//// �����¼����󣨽���int��string������
//SubscribeEvent<int, std::string> logger;
//
//// �����¼������ض������ƣ�
//auto sub1 = logger.subscribe([](int code, const std::string& msg) {
//    std::cout << "[Log1] Code: " << code << ", Msg: " << msg << std::endl;
//    });
//
//auto sub2 = logger.subscribe([](int code, std::string msg) {
//    std::cout << "[Log2] Code: " << code << ", Msg: " << std::move(msg) << std::endl;
//    });
//
//// �����¼��������ᱻ����ת����
//logger(200, "Request successful");
//
//// ȡ���ڶ�������
//logger.unsubscribe(sub2);
//
//// �ٴη����¼���ֻ�е�һ�����Ļᱻ������
//logger(404, "Not Found");
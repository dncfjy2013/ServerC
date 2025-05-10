#pragma once
#include <string>
#include <ctime>
#include "LogLevel.h"

// ����LogMessage�࣬����C#�е�LogMessage�ṹ��
class LogMessage {
private:
    std::chrono::system_clock::time_point time_point;
    LogLevel level;
    std::string message;
    std::thread::id threadId;
    std::string threadName;

public:
    // ���캯��
    LogMessage(std::chrono::system_clock::time_point ts, LogLevel lvl, const std::string& msg, std::thread::id tid, const std::string& tname)
        : time_point(ts), level(lvl), message(msg), threadId(tid), threadName(tname) {}

    // ��ȡʱ���
    std::chrono::system_clock::time_point getTimestamp() const {
        return time_point;
    }

    // ��ȡ��־����
    LogLevel getLevel() const {
        return level;
    }

    // ��ȡ��־��Ϣ
    std::string getMessage() const {
        return message;
    }

    // ��ȡ�߳�ID
    std::thread::id getThreadId() const {
        return threadId;
    }

    // ��ȡ�߳�����
    std::string getThreadName() const {
        return threadName;
    }
};
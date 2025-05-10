#pragma once
#include <string>
#include <ctime>
#include "LogLevel.h"

// 定义LogMessage类，类似C#中的LogMessage结构体
class LogMessage {
private:
    std::chrono::system_clock::time_point time_point;
    LogLevel level;
    std::string message;
    std::thread::id threadId;
    std::string threadName;

public:
    // 构造函数
    LogMessage(std::chrono::system_clock::time_point ts, LogLevel lvl, const std::string& msg, std::thread::id tid, const std::string& tname)
        : time_point(ts), level(lvl), message(msg), threadId(tid), threadName(tname) {}

    // 获取时间戳
    std::chrono::system_clock::time_point getTimestamp() const {
        return time_point;
    }

    // 获取日志级别
    LogLevel getLevel() const {
        return level;
    }

    // 获取日志消息
    std::string getMessage() const {
        return message;
    }

    // 获取线程ID
    std::thread::id getThreadId() const {
        return threadId;
    }

    // 获取线程名称
    std::string getThreadName() const {
        return threadName;
    }
};
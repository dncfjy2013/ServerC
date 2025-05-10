#pragma once

#include "Common/LoggerConfig.hpp"
#include "AbstarctLogger.hpp"
#include "Common/LogMessage.hpp"
#include <iostream>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <chrono>
#include <atomic>
#include <functional>
#include <memory>
#include <ctime>
#include "Common/LogLevel.hpp"
#include "../Common/Helpers/DateTimeHelper.hpp"


// 日志记录器实例类
class LoggerInstance : public AbstractLogger {
private:
    static std::shared_ptr<LoggerInstance> _instance;
    std::queue<LogMessage> _logQueue;
    std::mutex _queueMutex;
    std::condition_variable _queueCV;
    std::thread _logWriterThread;
    bool _isRunning;

    LoggerInstance() : AbstractLogger(std::make_shared<LoggerConfig>()), _isRunning(false) {
        if (getEnableAsyncWriting()) {
            _isRunning = true;
            _logWriterThread = std::thread([this]() {
                ProcessLogQueue();
            });
        }
    }

    void Log(LogLevel level, const std::string& message) {
        if (level < getConsoleLogLevel() && level < getFileLogLevel())
            return;

        auto now = std::chrono::system_clock::now();
        auto threadId = std::this_thread::get_id();
        std::string threadName = "Unknown";

        LogMessage logMessage(now, level, message, threadId, threadName);

        // 同步处理控制台输出
        if (level >= getConsoleLogLevel()) {
            WriteToConsole(logMessage);
        }

        // 异步处理文件输出
        if (getEnableAsyncWriting() && level >= getFileLogLevel()) {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _logQueue.push(logMessage);
            _queueCV.notify_one();
        }
        else {

        }
    }

    void ProcessLogQueue() {
        while (true) {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _queueCV.wait(lock, [this]() {
                return _logQueue.size() > 0 || getCancellationToken()->load();
            });

            while (!_logQueue.empty()) {
                LogMessage message = _logQueue.front();
                _logQueue.pop();
                lock.unlock();
                WriteToFile(message);
                lock.lock();
            }

            if (getCancellationToken()->load()) {
                break;
            }
        }
    }

    void WriteToConsole(const LogMessage& message) {
        ConsoleColor color = GetConsoleColor(message.getLevel());
        // 这里简单模拟设置控制台颜色，实际可能需要根据平台进行调整
        // 例如在Windows上使用SetConsoleTextAttribute，在Linux上使用ANSI转义序列
        std::cout << FormatMessage(message, "CONSOLE") << std::endl;
    }

    void WriteToFile(const LogMessage& message) {
        try {
            std::ofstream file(getLogFilePath(), std::ios::app);
            file << FormatMessage(message, "FILE") << std::endl;
            file.close();
        }
        catch (const std::exception& ex) {
            std::cerr << "Failed to write log to file: " << ex.what() << std::endl;
        }
    }

    std::string FormatMessage(const LogMessage& message, const std::string& target) {
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%m:%S.%f", &DateTimeUtils::to_tm(message.getTimestamp()));

        return std::string(buffer) + " [" + GetLogLevelString(message.getLevel()) + "] [Thread: " +
            threadIdToStrByHash(message.getThreadId()) + "/" + message.getThreadName() + "] " + message.getMessage();
    }

    std::string threadIdToStrByHash(const std::thread::id& tid) {
        std::size_t hashValue = std::hash<std::thread::id>{}(tid);
        return std::to_string(hashValue);
    }

    ConsoleColor GetConsoleColor(LogLevel level) {
        // 这里简单返回一个默认颜色，实际需要根据平台设置控制台颜色
        switch (level) {
        case LogLevel::Critical:
            // 假设这里返回一个表示深红色的枚举值，实际需要设置控制台颜色
            return ConsoleColor::DarkRed; 
        case LogLevel::Error:
            return ConsoleColor::DarkMagenta;
        case LogLevel::Warning:
            return ConsoleColor::DarkYellow;
        case LogLevel::Information:
            return ConsoleColor::DarkGreen;
        case LogLevel::Debug:
            return ConsoleColor::DarkCyan;
        case LogLevel::Trace:
            return ConsoleColor::DarkGray;
        default:
            return ConsoleColor::Gray;
        }
    }

    std::string GetLogLevelString(LogLevel level) {
        switch (level) {
        case LogLevel::Critical: return "CRITICAL";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Information: return "INFORMATION";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        default: return "UNKNOWN";
        }
    }

public:
    static LoggerInstance& GetInstance() {
        static std::once_flag flag;
        std::call_once(flag, []() {
            _instance = std::make_shared<LoggerInstance>();
        });
        return *_instance;
    }

    void LogTrace(const std::string& message) {
        Log(LogLevel::Trace, message);
    }

    void LogDebug(const std::string& message) {
        Log(LogLevel::Debug, message);
    }

    void LogInformation(const std::string& message) {
        Log(LogLevel::Information, message);
    }

    void LogWarning(const std::string& message) {
        Log(LogLevel::Warning, message);
    }

    void LogError(const std::string& message) {
        Log(LogLevel::Error, message);
    }

    void LogCritical(const std::string& message) {
        Log(LogLevel::Critical, message);
    }

    void Dispose() {
        if (getDispose()->load()) return;
        getCancellationToken()->store(true);
        _queueCV.notify_all();

        if (_logWriterThread.joinable()) {
            _logWriterThread.join();
        }

        getDispose()->store(true);
    }
};

std::shared_ptr<LoggerInstance> LoggerInstance::_instance;

// 简单模拟控制台颜色枚举
enum class ConsoleColor {
    DarkRed,
    DarkMagenta,
    DarkYellow,
    DarkGreen,
    DarkCyan,
    DarkGray,
    Gray
};
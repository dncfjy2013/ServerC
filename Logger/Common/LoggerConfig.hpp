#pragma once
#include "LogLevel.h"
#include <string>


class LoggerConfig {
private:
    LogLevel consoleLogLevel;
    LogLevel fileLogLevel;
    std::string logFilePath;
    bool enableAsyncWriting;

public:
    // 默认构造函数，使用默认值初始化
    LoggerConfig()
        : consoleLogLevel(LogLevel::Trace),
        fileLogLevel(LogLevel::Information),
        logFilePath("application.log"),
        enableAsyncWriting(true) {}

    // 获取控制台日志级别
    LogLevel getConsoleLogLevel() const {
        return consoleLogLevel;
    }

    // 设置控制台日志级别
    void setConsoleLogLevel(LogLevel level) {
        consoleLogLevel = level;
    }

    // 获取文件日志级别
    LogLevel getFileLogLevel() const {
        return fileLogLevel;
    }

    // 设置文件日志级别
    void setFileLogLevel(LogLevel level) {
        fileLogLevel = level;
    }

    // 获取日志文件路径
    const std::string& getLogFilePath() const {
        return logFilePath;
    }

    // 设置日志文件路径
    void setLogFilePath(const std::string& path) {
        logFilePath = path;
    }

    // 获取是否启用异步写入
    bool isAsyncWritingEnabled() const {
        return enableAsyncWriting;
    }

    // 设置是否启用异步写入
    void setEnableAsyncWriting(bool enable) {
        enableAsyncWriting = enable;
    }
};
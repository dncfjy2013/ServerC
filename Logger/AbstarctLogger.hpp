#include <string>
#include <memory>
#include <atomic>
#include <stdexcept>
#include "Common/LogLevel.hpp"
#include "Common/LoggerConfig.hpp"


// 抽象日志记录器类
class AbstractLogger {
private:
    std::shared_ptr<LoggerConfig> _config;
    std::shared_ptr<std::atomic<bool>> _cts;
    std::shared_ptr<std::atomic<bool>> _isDisposed;

public:
    // 构造函数，接受LoggerConfig的智能指针
    AbstractLogger(std::shared_ptr<LoggerConfig> config) : _config(config), _isDisposed(false) {
        if (!config) {
            throw std::invalid_argument("config cannot be null");
        }
        _cts = std::make_shared<std::atomic<bool>>(false);
    }

    // 获取控制台日志级别
    LogLevel getConsoleLogLevel() const {
        return _config->getConsoleLogLevel();
    }

    // 设置控制台日志级别
    void setConsoleLogLevel(LogLevel level) {
        _config->setConsoleLogLevel(level);
    }

    // 获取文件日志级别
    LogLevel getFileLogLevel() const {
        return _config->getFileLogLevel();
    }

    // 设置文件日志级别
    void setFileLogLevel(LogLevel level) {
        _config->setFileLogLevel(level);
    }

    // 获取日志文件路径
    std::string getLogFilePath() const {
        return _config->getLogFilePath();
    }

    // 设置日志文件路径
    void setLogFilePath(const std::string& path) {
        _config->setLogFilePath(path);
    }

    // 获取是否启用异步写入
    bool getEnableAsyncWriting() const {
        return _config->isAsyncWritingEnabled();
    }

    // 设置是否启用异步写入
    void setEnableAsyncWriting(bool enable) {
        _config->setEnableAsyncWriting(enable);
    }

    // 抽象方法：记录Trace级别的日志
    virtual void LogTrace(const std::string& message) = 0;

    // 抽象方法：记录Debug级别的日志
    virtual void LogDebug(const std::string& message) = 0;

    // 抽象方法：记录Information级别的日志
    virtual void LogInformation(const std::string& message) = 0;

    // 抽象方法：记录Warning级别的日志
    virtual void LogWarning(const std::string& message) = 0;

    // 抽象方法：记录Error级别的日志
    virtual void LogError(const std::string& message) = 0;

    // 抽象方法：记录Critical级别的日志
    virtual void LogCritical(const std::string& message) = 0;

    // 释放资源的虚方法
    virtual void Dispose() {
        if (_isDisposed->load()) return;
        _cts->store(true);
        _isDisposed->store(true);
    }

    // 获取取消令牌
    std::shared_ptr<std::atomic<bool>> getCancellationToken() const {
        return _cts;
    }

    std::shared_ptr <std::atomic<bool>> getDispose() const {
        return _isDisposed;
    }
};
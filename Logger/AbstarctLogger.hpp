#include <string>
#include <memory>
#include <atomic>
#include <stdexcept>
#include "Common/LogLevel.hpp"
#include "Common/LoggerConfig.hpp"


// ������־��¼����
class AbstractLogger {
private:
    std::shared_ptr<LoggerConfig> _config;
    std::shared_ptr<std::atomic<bool>> _cts;
    std::shared_ptr<std::atomic<bool>> _isDisposed;

public:
    // ���캯��������LoggerConfig������ָ��
    AbstractLogger(std::shared_ptr<LoggerConfig> config) : _config(config), _isDisposed(false) {
        if (!config) {
            throw std::invalid_argument("config cannot be null");
        }
        _cts = std::make_shared<std::atomic<bool>>(false);
    }

    // ��ȡ����̨��־����
    LogLevel getConsoleLogLevel() const {
        return _config->getConsoleLogLevel();
    }

    // ���ÿ���̨��־����
    void setConsoleLogLevel(LogLevel level) {
        _config->setConsoleLogLevel(level);
    }

    // ��ȡ�ļ���־����
    LogLevel getFileLogLevel() const {
        return _config->getFileLogLevel();
    }

    // �����ļ���־����
    void setFileLogLevel(LogLevel level) {
        _config->setFileLogLevel(level);
    }

    // ��ȡ��־�ļ�·��
    std::string getLogFilePath() const {
        return _config->getLogFilePath();
    }

    // ������־�ļ�·��
    void setLogFilePath(const std::string& path) {
        _config->setLogFilePath(path);
    }

    // ��ȡ�Ƿ������첽д��
    bool getEnableAsyncWriting() const {
        return _config->isAsyncWritingEnabled();
    }

    // �����Ƿ������첽д��
    void setEnableAsyncWriting(bool enable) {
        _config->setEnableAsyncWriting(enable);
    }

    // ���󷽷�����¼Trace�������־
    virtual void LogTrace(const std::string& message) = 0;

    // ���󷽷�����¼Debug�������־
    virtual void LogDebug(const std::string& message) = 0;

    // ���󷽷�����¼Information�������־
    virtual void LogInformation(const std::string& message) = 0;

    // ���󷽷�����¼Warning�������־
    virtual void LogWarning(const std::string& message) = 0;

    // ���󷽷�����¼Error�������־
    virtual void LogError(const std::string& message) = 0;

    // ���󷽷�����¼Critical�������־
    virtual void LogCritical(const std::string& message) = 0;

    // �ͷ���Դ���鷽��
    virtual void Dispose() {
        if (_isDisposed->load()) return;
        _cts->store(true);
        _isDisposed->store(true);
    }

    // ��ȡȡ������
    std::shared_ptr<std::atomic<bool>> getCancellationToken() const {
        return _cts;
    }

    std::shared_ptr <std::atomic<bool>> getDispose() const {
        return _isDisposed;
    }
};
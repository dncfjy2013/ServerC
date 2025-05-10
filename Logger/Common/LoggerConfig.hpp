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
    // Ĭ�Ϲ��캯����ʹ��Ĭ��ֵ��ʼ��
    LoggerConfig()
        : consoleLogLevel(LogLevel::Trace),
        fileLogLevel(LogLevel::Information),
        logFilePath("application.log"),
        enableAsyncWriting(true) {}

    // ��ȡ����̨��־����
    LogLevel getConsoleLogLevel() const {
        return consoleLogLevel;
    }

    // ���ÿ���̨��־����
    void setConsoleLogLevel(LogLevel level) {
        consoleLogLevel = level;
    }

    // ��ȡ�ļ���־����
    LogLevel getFileLogLevel() const {
        return fileLogLevel;
    }

    // �����ļ���־����
    void setFileLogLevel(LogLevel level) {
        fileLogLevel = level;
    }

    // ��ȡ��־�ļ�·��
    const std::string& getLogFilePath() const {
        return logFilePath;
    }

    // ������־�ļ�·��
    void setLogFilePath(const std::string& path) {
        logFilePath = path;
    }

    // ��ȡ�Ƿ������첽д��
    bool isAsyncWritingEnabled() const {
        return enableAsyncWriting;
    }

    // �����Ƿ������첽д��
    void setEnableAsyncWriting(bool enable) {
        enableAsyncWriting = enable;
    }
};
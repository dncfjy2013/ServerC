#pragma once
#include <stdexcept>
#include <string>
#include <exception>

class ApiException : public std::runtime_error {
private:
    int errorCode_;
    std::string errorDetails_;
    std::exception_ptr innerException_;  // ��춴惦�Ȳ�����

public:
    // ���A���캯�����H�e�`�a����Ϣ��
    explicit ApiException(int errorCode, const std::string& message)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(),
        innerException_(nullptr) {}

    // ��Ԕ���e�`��Ϣ�Ę��캯��
    ApiException(int errorCode, const std::string& message, const std::string& errorDetails)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(errorDetails),
        innerException_(nullptr) {}

    // ���Ȳ������Ę��캯��
    ApiException(int errorCode, const std::string& message, std::exception_ptr innerException)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(),
        innerException_(innerException) {}

    // �������캯������Ԕ����Ϣ�̓Ȳ�������
    ApiException(int errorCode, const std::string& message,
        const std::string& errorDetails, std::exception_ptr innerException)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(errorDetails),
        innerException_(innerException) {}

    // �@ȡ�e�`�a
    int errorCode() const noexcept {
        return errorCode_;
    }

    // �@ȡԔ���e�`��Ϣ
    const std::string& errorDetails() const noexcept {
        return errorDetails_;
    }

    // �@ȡ�Ȳ�������C++���Ќ��F��
    std::exception_ptr innerException() const noexcept {
        return innerException_;
    }

    // �@ȡ����e�`��Ϣ���،���std::exception��
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};
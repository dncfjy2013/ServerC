#pragma once
#include <stdexcept>
#include <string>
#include <exception>

class ApiException : public std::runtime_error {
private:
    int errorCode_;
    std::string errorDetails_;
    std::exception_ptr innerException_;  // 用於存儲內部異常

public:
    // 基礎構造函數（僅錯誤碼和消息）
    explicit ApiException(int errorCode, const std::string& message)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(),
        innerException_(nullptr) {}

    // 帶詳細錯誤信息的構造函數
    ApiException(int errorCode, const std::string& message, const std::string& errorDetails)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(errorDetails),
        innerException_(nullptr) {}

    // 帶內部異常的構造函數
    ApiException(int errorCode, const std::string& message, std::exception_ptr innerException)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(),
        innerException_(innerException) {}

    // 完整構造函數（含詳細信息和內部異常）
    ApiException(int errorCode, const std::string& message,
        const std::string& errorDetails, std::exception_ptr innerException)
        : std::runtime_error(message),
        errorCode_(errorCode),
        errorDetails_(errorDetails),
        innerException_(innerException) {}

    // 獲取錯誤碼
    int errorCode() const noexcept {
        return errorCode_;
    }

    // 獲取詳細錯誤信息
    const std::string& errorDetails() const noexcept {
        return errorDetails_;
    }

    // 獲取內部異常（C++特有實現）
    std::exception_ptr innerException() const noexcept {
        return innerException_;
    }

    // 獲取基類錯誤信息（重寫自std::exception）
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};
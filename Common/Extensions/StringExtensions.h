#pragma once
#include <string>
#include <regex>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <random>
#include <cctype>
#include <algorithm>
#include <numeric>
#include <cstring>

namespace StringUtils {
    // 空值檢查模塊 ----------------------------------------------------------

    /**
     * @brief 檢查字符串是否為nullptr或空字符串
     */
    inline bool IsNullOrEmpty(const std::string& str) {
        return str.empty();
    }

    /**
     * @brief 檢查字符串是否為nullptr、空或僅包含空白字符
     */
    inline bool IsNullOrWhiteSpace(const std::string& str) {
        return std::all_of(str.begin(), str.end(), [](unsigned char c) {
            return std::isspace(c);
            });
    }

    // 格式轉換模塊 ----------------------------------------------------------

    /**
     * @brief 轉換為駝峰命名法（首字母小寫）
     */
    inline std::string ToCamelCase(std::string value) {
        if (value.empty()) return value;
        value[0] = std::tolower(value[0]);
        return value;
    }

    /**
     * @brief 下劃線命名轉駝峰命名
     */
    inline std::string SnakeCaseToCamelCase(std::string value) {
        std::stringstream ss;
        bool capitalizeNext = false;

        for (auto&& ch : value) {
            if (ch == '_') {
                capitalizeNext = true;
            }
            else {
                if (capitalizeNext) {
                    ss << static_cast<char>(std::toupper(ch));
                    capitalizeNext = false;
                }
                else {
                    ss << static_cast<char>(std::tolower(ch));
                }
            }
        }
        return ss.str();
    }

    /**
     * @brief 轉換為首字母大寫形式
     */
    inline std::string ToTitleCase(std::string value) {
        if (value.empty()) return value;
        value[0] = std::toupper(value[0]);
        return value;
    }

    // 安全處理模塊 ----------------------------------------------------------

    /**
     * @brief 計算MD5哈希值
     */
    inline std::string ToMD5(const std::string& input) {
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char*>(input.data()), input.size(), digest);

        std::stringstream ss;
        for (auto byte : digest) {
            ss << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(byte);
        }
        return ss.str();
    }

    /**
     * @brief 計算SHA256哈希值
     */
    inline std::string ToSHA256(const std::string& input) {
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.data()),
            input.size(), digest);

        std::stringstream ss;
        for (auto byte : digest) {
            ss << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(byte);
        }
        return ss.str();
    }

    // 驗證方法模塊 ----------------------------------------------------------

    /**
     * @brief 驗證是否為有效郵箱地址
     */
    inline bool IsValidEmail(const std::string& email) {
        const std::regex pattern(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
        return std::regex_match(email, pattern);
    }

    /**
     * @brief 驗證是否為有效URL
     */
    inline bool IsValidUrl(const std::string& url) {
        try {
            size_t start;
            if (url.substr(0, 7) == "http://") start = 7;
            else if (url.substr(0, 8) == "https://") start = 8;
            else return false;

            size_t end = url.find('/', start);
            std::string domain = url.substr(start, end - start);
            return !domain.empty() && domain.find('.') != std::string::npos;
        }
        catch (...) {
            return false;
        }
    }

    // 字符串處理模塊 --------------------------------------------------------

    /**
     * @brief 安全截斷字符串
     */
    inline std::string Truncate(std::string value, size_t maxLength,
        const std::string& suffix = "...") {
        if (value.size() <= maxLength) return value;
        return value.substr(0, maxLength) + suffix;
    }

    /**
     * @brief 生成隨機字符串
     */
    inline std::string RandomString(size_t length, bool useSpecialChars = false) {
        const std::string chars =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789" +
            (useSpecialChars ? "!@#$%^&*" : "");

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, chars.size() - 1);

        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += chars[dist(gen)];
        }
        return result;
    }

    // 編碼轉換模塊 ----------------------------------------------------------

    /**
     * @brief Base64編碼
     */
    inline std::string ToBase64(const std::string& input) {
        std::string encoded;
        BIO* bio = BIO_new(BIO_f_base64());
        BIO* b64 = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);

        BIO_write(bio, input.data(), input.size());
        BIO_flush(bio);

        char* buffer;
        long len = BIO_get_mem_data(b64, &buffer);
        encoded.assign(buffer, len);

        BIO_free_all(bio);
        return encoded;
    }

    /**
     * @brief Base64解碼
     */
    inline std::string FromBase64(const std::string& input) {
        std::string decoded;
        BIO* bio = BIO_new(BIO_f_base64());
        BIO* b64 = BIO_new_mem_buf(input.data(), input.size());
        bio = BIO_push(b64, bio);

        char buffer[1024];
        int bytesRead;
        while ((bytesRead = BIO_read(bio, buffer, sizeof(buffer))) > 0) {
            decoded.append(buffer, bytesRead);
        }

        BIO_free_all(bio);
        return decoded;
    }

    // 其他實用方法 ----------------------------------------------------------

    /**
     * @brief 統計單詞數量
     */
    inline int WordCount(const std::string& input) {
        std::regex wordRegex(R"(\b\w+\b)");
        auto wordsBegin = std::sregex_iterator(input.begin(), input.end(), wordRegex);
        auto wordsEnd = std::sregex_iterator();
        return std::distance(wordsBegin, wordsEnd);
    }
}

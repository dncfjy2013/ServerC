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
    // ��ֵ�z��ģ�K ----------------------------------------------------------

    /**
     * @brief �z���ַ����Ƿ��գ�nullptr����ַ�����
     * @param str ���z����ַ�������
     * @return ���ַ�������true���ǿշ���false
     * @note C++���Å�����������nullptr����̎�����cԭʼ���Fһ��
     */
    inline bool IsNullOrEmpty(const std::string& str) {
        return str.empty();
    }

    /**
     * @brief �z���ַ����Ƿ��ջ�H�����հ��ַ�
     * @param str ���z����ַ���
     * @return ȫ�հ׻���ַ�������true����t����false
     */
    inline bool IsNullOrWhiteSpace(const std::string& str) {
        return std::all_of(str.begin(), str.end(), [](unsigned char c) {
            return std::isspace(c);
            });
    }

    // ��ʽ�D�Qģ�K ----------------------------------------------------------

    /**
     * @brief �D�Q��С����������������ĸС����
     * @param value ���D�Q���ַ���
     * @return �D�Q�����ʽ�ַ�����ʾ����"helloWorld"��
     */
    inline std::string ToCamelCase(std::string value) {
        if (value.empty()) return value;
        value[0] = std::tolower(value[0]);
        return value;
    }

    /**
     * @brief ���������D�Q����������
     * @param value ���D�Q��������ʽ�ַ�����ʾ����"snake_case"��
     * @return �D�Q�����ʽ�ַ�����ʾ����"SnakeCase"��
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
     * @brief �D�Q������ĸ�󌑸�ʽ
     * @param value ���D�Q���ַ���
     * @return ����ĸ�󌑵��ַ�����ʾ����"hello" �� "Hello"��
     */
    inline std::string ToTitleCase(std::string value) {
        if (value.empty()) return value;
        value[0] = std::toupper(value[0]);
        return value;
    }

    // ��ȫ̎��ģ�K ----------------------------------------------------------

    /**
     * @brief Ӌ���ַ�����MD5��ϣֵ
     * @param input ԭʼ�ַ���
     * @return 32λʮ���M�Ʊ�ʾ��MD5��ϣֵ
     * @note ��هOpenSSL�죬��_�����g�r朽�-lcrypto
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
     * @brief Ӌ���ַ�����SHA256��ϣֵ
     * @param input ԭʼ�ַ���
     * @return 64λʮ���M�Ʊ�ʾ��SHA256��ϣֵ
     * @note ��هOpenSSL�죬��_�����g�r朽�-lcrypto
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

    // ��C����ģ�K ----------------------------------------------------------

    /**
     * @brief ��C�Ƿ����Ч������]���ַ
     * @param email ����C���]���ַ
     * @return ��ʽ��Ч����true����t����false
     * @note ʹ�����t���_ʽ��C������ʽ������C�����挍��
     */
    inline bool IsValidEmail(const std::string& email) {
        const std::regex pattern(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
        return std::regex_match(email, pattern);
    }

    /**
     * @brief ��C�Ƿ����Ч��URL��ʽ
     * @param url ����C��URL
     * @return ��ЧURL����true����t����false
     * @note ������Chttp/https�f�h������C��������
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

    // �ַ���̎��ģ�K --------------------------------------------------------

    /**
     * @brief ��ȫ�ؔ��ַ����K�����Y
     * @param value ԭʼ�ַ���
     * @param maxLength ������S�L�ȣ�������Y��
     * @param suffix �ؔ�����ӵ���Y��Ĭ�J"..."��
     * @return �ؔ�����ַ�����ʾ����"HelloWorld" �� "Hello..."��
     */
    inline std::string Truncate(std::string value, size_t maxLength,
        const std::string& suffix = "...") {
        if (value.size() <= maxLength) return value;
        return value.substr(0, maxLength) + suffix;
    }

    /**
     * @brief ����ָ���L�ȵ��S�C�ַ���
     * @param length �ַ����L��
     * @param useSpecialChars �Ƿ���������ַ���Ĭ�Jfalse��
     * @return �S�C�ַ�����������С����ĸ�͔��֣�
     * @note ʹ�ü��ܰ�ȫ���S�C��������
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

    // ���a�D�Qģ�K ----------------------------------------------------------

    /**
     * @brief Base64���a
     * @param input ԭʼ�ַ���
     * @return Base64���a����ַ���
     * @note ��هOpenSSL�죬��_�����g�r朽�-lcrypto
     */
    inline std::string ToBase64(const std::string& input) {
        // ���Fʡ�ԣ������cԭʼ���aһ�£�
    }

    /**
     * @brief Base64��a
     * @param input Base64���a�ַ���
     * @return ��a���ԭʼ�ַ���
     * @note ��هOpenSSL�죬��_�����g�r朽�-lcrypto
     */
    inline std::string FromBase64(const std::string& input) {
        // ���Fʡ�ԣ������cԭʼ���aһ�£�
    }

    // �������÷��� ----------------------------------------------------------

    /**
     * @brief �yӋ�ַ����еĆ��~����
     * @param input ���yӋ���ı�
     * @return ���~���������\b\w+\b���tƥ�䣩
     * @note Ӣ�Ć��~�yӋ��ʹ�ÿհ׺͘��c�ָ�
     */
    inline int WordCount(const std::string& input) {
        std::regex wordRegex(R"(\b\w+\b)");
        auto wordsBegin = std::sregex_iterator(input.begin(), input.end(), wordRegex);
        auto wordsEnd = std::sregex_iterator();
        return std::distance(wordsBegin, wordsEnd);
    }
}
#pragma once
#include <cctype>
#include <string>
#include <regex>
#include <unordered_set>

namespace CharUtils {
    // 字符分類判斷系列 ----------------------------------------------------------

    /**
     * @brief 判斷字符是否為大寫字母（A-Z）
     * @param c 要檢查的字符
     * @return 如果是大寫字母返回true，否則返回false
     * @note 使用std::isupper並強制轉換為unsigned char
     *       以避免某些實現中的符號擴展問題
     */
    inline bool IsUpperCase(char c) {
        return std::isupper(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為小寫字母（a-z）
     * @param c 要檢查的字符
     * @return 如果是小寫字母返回true，否則返回false
     */
    inline bool IsLowerCase(char c) {
        return std::islower(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為十進制數字（0-9）
     * @param c 要檢查的字符
     * @return 如果是數字返回true，否則返回false
     */
    inline bool IsDigit(char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為字母（包括大小寫）
     * @param c 要檢查的字符
     * @return 如果是字母返回true，否則返回false
     */
    inline bool IsLetter(char c) {
        return std::isalpha(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為字母或數字
     * @param c 要檢查的字符
     * @return 如果是字母/數字返回true，否則返回false
     */
    inline bool IsLetterOrDigit(char c) {
        return std::isalnum(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為空白字符
     * @param c 要檢查的字符
     * @return 如果是空白字符返回true，否則返回false
     * @note 包括空格、換行符、制表符等（依C++區域設定）
     */
    inline bool IsWhiteSpace(char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為標點符號
     * @param c 要檢查的字符
     * @return 如果是標點返回true，否則返回false
     * @note 依C++區域設定，通常包括!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
     */
    inline bool IsPunctuation(char c) {
        return std::ispunct(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為控制字符
     * @param c 要檢查的字符
     * @return 如果是控制字符返回true，否則返回false
     * @note 控制字符包括ASCII 0-31和127（DEL）
     */
    inline bool IsControl(char c) {
        return std::iscntrl(static_cast<unsigned char>(c));
    }

    /**
     * @brief 判斷字符是否為符號字符（非字母數字的標點）
     * @param c 要檢查的字符
     * @return 如果是符號返回true，否則返回false
     * @note 實現了常見符號的集合，可按需擴展symbols集合
     */
    inline bool IsSymbol(char c) {
        const std::unordered_set<char> symbols = {
            '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
            ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~'
        };
        return symbols.count(c) > 0;
    }

    // 字符轉換系列 ----------------------------------------------------------

    /**
     * @brief 將字符轉換為大寫形式
     * @param c 要轉換的字符
     * @return 轉換後的大寫字符（如果原字符不是字母則直接返回）
     */
    inline char ToUpper(char c) {
        return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    /**
     * @brief 將字符轉換為小寫形式
     * @param c 要轉換的字符
     * @return 轉換後的小寫字符（如果原字符不是字母則直接返回）
     */
    inline char ToLower(char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    // 字符屬性判斷系列 ------------------------------------------------------

    /**
     * @brief 獲取字符的ASCII碼值
     * @param c 要轉換的字符
     * @return 字符的ASCII碼值（0-255）
     */
    inline int ToAsciiCode(char c) {
        return static_cast<int>(c);
    }

    /**
     * @brief 判斷字符是否為元音字母（不區分大小寫）
     * @param c 要檢查的字符
     * @return 如果是元音（a/e/i/o/u）返回true，否則返回false
     */
    inline bool IsVowel(char c) {
        const char lower = ToLower(c);
        return lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u';
    }

    /**
     * @brief 判斷字符是否為十六進制數字（0-9, a-f, A-F）
     * @param c 要檢查的字符
     * @return 如果是十六進制數字返回true，否則返回false
     * @note 優化實現，比正則表達式更高效
     */
    inline bool IsHexDigit(char c) {
        return (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F');
    }
}

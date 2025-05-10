#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <cstdint>

namespace IntUtils {
    // 數值屬性判斷 ----------------------------------------------------------

    /// @brief 判断整数是否为偶数
    /// @param number 待检测的整数
    /// @return 偶数返回true，否则返回false
    inline bool IsEven(int number) {
        return number % 2 == 0;
    }

    /// @brief 判断整数是否为奇数
    /// @param number 待检测的整数
    /// @return 奇数返回true，否则返回false
    inline bool IsOdd(int number) {
        return number % 2 != 0;
    }

    /// @brief 判断整数是否为质数（素数）
    /// @param number 待检测的整数（小于2的数直接返回false）
    /// @return 质数返回true，否则返回false
    inline bool IsPrime(int number) {
        if (number < 2) return false;
        for (int i = 2; i <= static_cast<int>(std::sqrt(number)); ++i) {
            if (number % i == 0) return false;
        }
        return true;
    }

    // 數學運算 ------------------------------------------------------------

    /// @brief 计算非负整数的阶乘
    /// @param number 非负整数（n >= 0）
    /// @return n! 的值
    /// @throws std::invalid_argument 当输入为负数时抛出异常
    inline int64_t Factorial(int number) {
        if (number < 0) {
            throw std::invalid_argument("階乘不能為負數");
        }
        int64_t result = 1;
        for (int i = 2; i <= number; ++i) {
            result *= i;
        }
        return result;
    }

    // 序列生成 ------------------------------------------------------------

    /// @brief 生成整数序列（支持正序/倒序）
    /// @param start 序列起始值
    /// @param end 序列结束值
    /// @return 包含从start到end（含）所有整数的vector
    /// @note 当start <= end时生成正序序列，否则生成倒序序列
    inline std::vector<int> To(int start, int end) {
        std::vector<int> sequence;
        if (start <= end) {
            for (int i = start; i <= end; ++i) sequence.push_back(i);
        }
        else {
            for (int i = start; i >= end; --i) sequence.push_back(i);
        }
        return sequence;
    }

    // 數值轉換 ------------------------------------------------------------

    /// @brief 将整数转换为罗马数字表示
    /// @param number 要转换的整数（范围：1-3999）
    /// @return 罗马数字字符串
    /// @throws std::invalid_argument 当输入超出有效范围时抛出异常
    inline std::string ToRomanNumeral(int number) {
        if (number < 1 || number > 3999) {
            throw std::invalid_argument("羅馬數字只能表示 1 到 3999 之間的整數");
        }
        const std::string thousands[] = { "", "M", "MM", "MMM" };
        const std::string hundreds[] = { "", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM" };
        const std::string tens[] = { "", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC" };
        const std::string ones[] = { "", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" };

        return thousands[number / 1000] +
            hundreds[(number % 1000) / 100] +
            tens[(number % 100) / 10] +
            ones[number % 10];
    }

    /// @brief 计算整数各位数字之和
    /// @param number 任意整数（负数会取绝对值处理）
    /// @return 各位数字之和（例如：1234 → 1+2+3+4=10）
    inline int SumOfDigits(int number) {
        number = std::abs(number);
        int sum = 0;
        do {
            sum += number % 10;
            number /= 10;
        } while (number > 0);
        return sum;
    }

    /// @brief 反转整数的数字顺序
    /// @param number 任意整数（负数会保留符号）
    /// @return 反转后的整数（例如：123 → 321，-456 → -654）
    inline int ReverseDigits(int number) {
        bool isNegative = number < 0;
        number = std::abs(number);
        int reversed = 0;
        do {
            reversed = reversed * 10 + number % 10;
            number /= 10;
        } while (number > 0);
        return isNegative ? -reversed : reversed;
    }

    /// @brief 判断整数是否为回文数
    /// @param number 待检测的整数
    /// @return 是回文数返回true，否则返回false
    /// @note 回文数指正读反读都相同的数（例如：121，-454-）
    inline bool IsPalindrome(int number) {
        return number == ReverseDigits(number);
    }

    // 進制轉換 ------------------------------------------------------------

    /// @brief 将整数转换为二进制字符串（无前导零）
    /// @param number 要转换的整数
    /// @return 二进制字符串表示（例如：5 → "101"）
    inline std::string ToBinary(int number) {
        return std::bitset<32>(number).to_string().substr(
            std::bitset<32>(number).to_string().find('1')
        );
    }

    /// @brief 将整数转换为八进制字符串（无前导零）
    /// @param number 要转换的整数
    /// @return 八进制字符串表示（例如：10 → "12"）
    inline std::string ToOctal(int number) {
        std::ostringstream oss;
        oss << std::oct << number;
        std::string result = oss.str();
        return result.empty() ? "0" : result.substr(result.find_first_not_of('0'));
    }

    /// @brief 将整数转换为十六进制字符串（大写，无前导零）
    /// @param number 要转换的整数
    /// @return 十六进制字符串表示（例如：255 → "FF"）
    inline std::string ToHexadecimal(int number) {
        std::ostringstream oss;
        oss << std::uppercase << std::hex << static_cast<uint32_t>(number);
        std::string result = oss.str();
        return result.empty() ? "0" : result.substr(result.find_first_not_of('0'));
    }

    /// @brief 将指定进制的字符串转换为整数
    /// @param value 要转换的字符串
    /// @param fromBase 进制基数（2-36）
    /// @return 转换后的整数
    /// @throws std::invalid_argument 当字符串包含无效字符时抛出异常
    inline int FromBaseString(const std::string& value, int fromBase) {
        return std::stoi(value, nullptr, fromBase);
    }
}
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
    // ��ֵ�����Д� ----------------------------------------------------------

    inline bool IsEven(int number) {
        return number % 2 == 0;
    }

    inline bool IsOdd(int number) {
        return number % 2 != 0;
    }

    inline bool IsPrime(int number) {
        if (number < 2) return false;
        for (int i = 2; i <= static_cast<int>(std::sqrt(number)); ++i) {
            if (number % i == 0) return false;
        }
        return true;
    }

    // ���W�\�� ------------------------------------------------------------

    inline int64_t Factorial(int number) {
        if (number < 0) {
            throw std::invalid_argument("�A�˲��ܞ�ؓ��");
        }
        int64_t result = 1;
        for (int i = 2; i <= number; ++i) {
            result *= i;
        }
        return result;
    }

    // �������� ------------------------------------------------------------

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

    // ��ֵ�D�Q ------------------------------------------------------------

    inline std::string ToRomanNumeral(int number) {
        if (number < 1 || number > 3999) {
            throw std::invalid_argument("�_�R����ֻ�ܱ�ʾ 1 �� 3999 ֮�g������");
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

    inline int SumOfDigits(int number) {
        number = std::abs(number);
        int sum = 0;
        do {
            sum += number % 10;
            number /= 10;
        } while (number > 0);
        return sum;
    }

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

    inline bool IsPalindrome(int number) {
        return number == ReverseDigits(number);
    }

    // �M���D�Q ------------------------------------------------------------

    inline std::string ToBinary(int number) {
        return std::bitset<32>(number).to_string().substr(
            std::bitset<32>(number).to_string().find('1')
        );
    }

    inline std::string ToOctal(int number) {
        std::ostringstream oss;
        oss << std::oct << number;
        std::string result = oss.str();
        return result.empty() ? "0" : result.substr(result.find_first_not_of('0'));
    }

    inline std::string ToHexadecimal(int number) {
        std::ostringstream oss;
        oss << std::uppercase << std::hex << static_cast<uint32_t>(number);
        std::string result = oss.str();
        return result.empty() ? "0" : result.substr(result.find_first_not_of('0'));
    }

    inline int FromBaseString(const std::string& value, int fromBase) {
        return std::stoi(value, nullptr, fromBase);
    }
}
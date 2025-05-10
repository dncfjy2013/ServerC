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

    /// @brief �ж������Ƿ�Ϊż��
    /// @param number ����������
    /// @return ż������true�����򷵻�false
    inline bool IsEven(int number) {
        return number % 2 == 0;
    }

    /// @brief �ж������Ƿ�Ϊ����
    /// @param number ����������
    /// @return ��������true�����򷵻�false
    inline bool IsOdd(int number) {
        return number % 2 != 0;
    }

    /// @brief �ж������Ƿ�Ϊ������������
    /// @param number ������������С��2����ֱ�ӷ���false��
    /// @return ��������true�����򷵻�false
    inline bool IsPrime(int number) {
        if (number < 2) return false;
        for (int i = 2; i <= static_cast<int>(std::sqrt(number)); ++i) {
            if (number % i == 0) return false;
        }
        return true;
    }

    // ���W�\�� ------------------------------------------------------------

    /// @brief ����Ǹ������Ľ׳�
    /// @param number �Ǹ�������n >= 0��
    /// @return n! ��ֵ
    /// @throws std::invalid_argument ������Ϊ����ʱ�׳��쳣
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

    /// @brief �����������У�֧������/����
    /// @param start ������ʼֵ
    /// @param end ���н���ֵ
    /// @return ������start��end����������������vector
    /// @note ��start <= endʱ�����������У��������ɵ�������
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

    /// @brief ������ת��Ϊ�������ֱ�ʾ
    /// @param number Ҫת������������Χ��1-3999��
    /// @return ���������ַ���
    /// @throws std::invalid_argument �����볬����Ч��Χʱ�׳��쳣
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

    /// @brief ����������λ����֮��
    /// @param number ����������������ȡ����ֵ����
    /// @return ��λ����֮�ͣ����磺1234 �� 1+2+3+4=10��
    inline int SumOfDigits(int number) {
        number = std::abs(number);
        int sum = 0;
        do {
            sum += number % 10;
            number /= 10;
        } while (number > 0);
        return sum;
    }

    /// @brief ��ת����������˳��
    /// @param number ���������������ᱣ�����ţ�
    /// @return ��ת������������磺123 �� 321��-456 �� -654��
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

    /// @brief �ж������Ƿ�Ϊ������
    /// @param number ����������
    /// @return �ǻ���������true�����򷵻�false
    /// @note ������ָ������������ͬ���������磺121��-454-��
    inline bool IsPalindrome(int number) {
        return number == ReverseDigits(number);
    }

    // �M���D�Q ------------------------------------------------------------

    /// @brief ������ת��Ϊ�������ַ�������ǰ���㣩
    /// @param number Ҫת��������
    /// @return �������ַ�����ʾ�����磺5 �� "101"��
    inline std::string ToBinary(int number) {
        return std::bitset<32>(number).to_string().substr(
            std::bitset<32>(number).to_string().find('1')
        );
    }

    /// @brief ������ת��Ϊ�˽����ַ�������ǰ���㣩
    /// @param number Ҫת��������
    /// @return �˽����ַ�����ʾ�����磺10 �� "12"��
    inline std::string ToOctal(int number) {
        std::ostringstream oss;
        oss << std::oct << number;
        std::string result = oss.str();
        return result.empty() ? "0" : result.substr(result.find_first_not_of('0'));
    }

    /// @brief ������ת��Ϊʮ�������ַ�������д����ǰ���㣩
    /// @param number Ҫת��������
    /// @return ʮ�������ַ�����ʾ�����磺255 �� "FF"��
    inline std::string ToHexadecimal(int number) {
        std::ostringstream oss;
        oss << std::uppercase << std::hex << static_cast<uint32_t>(number);
        std::string result = oss.str();
        return result.empty() ? "0" : result.substr(result.find_first_not_of('0'));
    }

    /// @brief ��ָ�����Ƶ��ַ���ת��Ϊ����
    /// @param value Ҫת�����ַ���
    /// @param fromBase ���ƻ�����2-36��
    /// @return ת���������
    /// @throws std::invalid_argument ���ַ���������Ч�ַ�ʱ�׳��쳣
    inline int FromBaseString(const std::string& value, int fromBase) {
        return std::stoi(value, nullptr, fromBase);
    }
}
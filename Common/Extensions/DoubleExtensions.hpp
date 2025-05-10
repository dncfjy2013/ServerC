#pragma once
#include <cmath>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <type_traits>

namespace DoubleUtils {
    // ��ֵ�z��c�����Д� ------------------------------------------------------

    /**
     * @brief �Д��p���ȸ��c���Ƿ������
     * @param number Ҫ�z��Ĕ�ֵ
     * @return �������������true����t����false
     * @note ʹ���Ē�������Ĳ�ֵ�c��С���ȱ��^
     */
    inline bool IsInteger(double number) {
        return std::abs(number - std::round(number)) <
            std::numeric_limits<double>::epsilon();
    }

    /**
     * @brief �Д��ɂ��p���Ȕ��Ƿ�������
     * @param number1 ��һ����
     * @param number2 �ڶ�����
     * @param tolerance �ݲ�ֵ��Ĭ�J��double���ȣ�
     * @return ������ȷ���true����t����false
     */
    inline bool IsApproximatelyEqual(double number1, double number2,
        double tolerance = std::numeric_limits<double>::epsilon()) {
        return std::abs(number1 - number2) < tolerance;
    }

    /**
     * @brief �z���Ƿ��ǔ�ֵ(NaN)
     */
    inline bool IsNaN(double number) {
        return std::isnan(number);
    }

    /**
     * @brief �z���Ƿ�����o�F��
     */
    inline bool IsPositiveInfinity(double number) {
        return std::isinf(number) && std::signbit(number) == 0;
    }

    /**
     * @brief �z���Ƿ��ؓ�o�F��
     */
    inline bool IsNegativeInfinity(double number) {
        return std::isinf(number) && std::signbit(number) != 0;
    }

    // ���W�\�� ----------------------------------------------------------------

    /**
     * @brief �Ē����뵽ָ��С��λ
     * @param decimalPlaces С��λ����0-15��
     * @return �\��Y��
     * @note ����С��λ���ȕr����ԭʼֵ
     */
    inline double RoundTo(double number, int decimalPlaces) {
        if (decimalPlaces < 0 || decimalPlaces > 15) return number;
        const double scale = std::pow(10.0, decimalPlaces);
        return std::round(number * scale) / scale;
    }

    /**
     * @brief ����ȡ��
     */
    inline double Ceiling(double number) {
        return std::ceil(number);
    }

    /**
     * @brief ����ȡ��
     */
    inline double Floor(double number) {
        return std::floor(number);
    }

    /**
     * @brief Ӌ��^��ֵ
     */
    inline double Abs(double number) {
        return std::abs(number);
    }

    /**
     * @brief Ӌ��ƽ��ֵ
     */
    inline double Square(double number) {
        return number * number;
    }

    /**
     * @brief Ӌ��ƽ����
     * @throws std::invalid_argument ��ݔ���ؓ���r����
     */
    inline double Sqrt(double number) {
        if (number < 0) {
            throw std::invalid_argument("���܌�ؓ����ƽ����");
        }
        return std::sqrt(number);
    }

    /**
     * @brief Ӌ���η�
     */
    inline double Pow(double number, double power) {
        return std::pow(number, power);
    }

    // ��ʽ�D�Q ----------------------------------------------------------------

    /**
     * @brief �D�Q��ٷֱ��ַ���
     * @param decimalPlaces С��λ����Ĭ�J2λ��
     * @return ��ʽ����İٷֱ��ַ�������"75.50%"��
     */
    inline std::string ToPercentageString(double number, int decimalPlaces = 2) {
        std::ostringstream stream;
        stream.precision(decimalPlaces);
        stream << std::fixed << number * 100 << "%";
        return stream.str();
    }
}

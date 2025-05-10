#pragma once
#include <cmath>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <type_traits>

namespace FloatUtils {
    // ��ֵ�z��c�����Д� ------------------------------------------------------

    /**
     * @brief �Д��ξ��ȸ��c���Ƿ������
     * @param number Ҫ�z��Ĕ�ֵ
     * @return ������������true����t����false
     * @note ʹ���Ē�������Ĳ�ֵ�c��С���ȱ��^
     */
    inline bool IsInteger(float number) {
        return std::abs(number - std::roundf(number)) <
            std::numeric_limits<float>::epsilon();
    }

    /**
     * @brief �Д��ɂ��ξ��Ȕ��Ƿ�������
     * @param tolerance �ݲ�ֵ��Ĭ�J��float���ȣ�
     * @return ������ȷ���true����t����false
     */
    inline bool IsApproximatelyEqual(float number1, float number2,
        float tolerance = std::numeric_limits<float>::epsilon()) {
        return std::abs(number1 - number2) < tolerance;
    }

    /**
     * @brief �z���Ƿ��ǔ�ֵ(NaN)
     */
    inline bool IsNaN(float number) {
        return std::isnan(number);
    }

    /**
     * @brief �z���Ƿ�����o�F��
     */
    inline bool IsPositiveInfinity(float number) {
        return std::isinf(number) && std::signbit(number) == 0;
    }

    /**
     * @brief �z���Ƿ��ؓ�o�F��
     */
    inline bool IsNegativeInfinity(float number) {
        return std::isinf(number) && std::signbit(number) != 0;
    }

    // ���W�\�� ----------------------------------------------------------------

    /**
     * @brief �Ē����뵽ָ��С��λ
     * @param decimalPlaces С��λ����0-7��
     * @return �\��Y��
     * @note ����С��λ���ȕr����ԭʼֵ
     */
    inline float RoundTo(float number, int decimalPlaces) {
        if (decimalPlaces < 0 || decimalPlaces > 7) return number;
        const float scale = std::powf(10.0f, decimalPlaces);
        return std::roundf(number * scale) / scale;
    }

    /**
     * @brief ����ȡ��
     */
    inline float Ceiling(float number) {
        return std::ceilf(number);
    }

    /**
     * @brief ����ȡ��
     */
    inline float Floor(float number) {
        return std::floorf(number);
    }

    /**
     * @brief Ӌ��^��ֵ
     */
    inline float Abs(float number) {
        return std::abs(number);
    }

    /**
     * @brief Ӌ��ƽ��ֵ
     */
    inline float Square(float number) {
        return number * number;
    }

    /**
     * @brief Ӌ��ƽ����
     * @throws std::invalid_argument ��ݔ���ؓ���r����
     */
    inline float Sqrt(float number) {
        if (number < 0) {
            throw std::invalid_argument("���܌�ؓ����ƽ����");
        }
        return std::sqrtf(number);
    }

    /**
     * @brief Ӌ���η�
     */
    inline float Pow(float number, float power) {
        return std::powf(number, power);
    }

    // ��ʽ�D�Q ----------------------------------------------------------------

    /**
     * @brief �D�Q��ٷֱ��ַ���
     * @param decimalPlaces С��λ����Ĭ�J2λ��
     * @return ��ʽ����İٷֱ��ַ�������"75.50%"��
     */
    inline std::string ToPercentageString(float number, int decimalPlaces = 2) {
        std::ostringstream stream;
        stream.precision(decimalPlaces);
        stream << std::fixed << number * 100.0f << "%";
        return stream.str();
    }

    // ����D�Q ----------------------------------------------------------------

    /**
     * @brief �D�Q���p���ȸ��c��
     */
    inline double ToDouble(float number) {
        return static_cast<double>(number);
    }

    /**
     * @brief �D�Q��������ֱ�ӽؔ�С�����֣�
     */
    inline int ToInt(float number) {
        return static_cast<int>(number);
    }
}
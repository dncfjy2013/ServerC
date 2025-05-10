#pragma once
#include <cmath>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <type_traits>

namespace FloatUtils {
    // 數值檢驗與屬性判斷 ------------------------------------------------------

    /**
     * @brief 判斷單精度浮點數是否為整數
     * @param number 要檢查的數值
     * @return 若為整數返回true，否則返回false
     * @note 使用四捨五入後的差值與最小精度比較
     */
    inline bool IsInteger(float number) {
        return std::abs(number - std::roundf(number)) <
            std::numeric_limits<float>::epsilon();
    }

    /**
     * @brief 判斷兩個單精度數是否近似相等
     * @param tolerance 容差值（默認為float精度）
     * @return 近似相等返回true，否則返回false
     */
    inline bool IsApproximatelyEqual(float number1, float number2,
        float tolerance = std::numeric_limits<float>::epsilon()) {
        return std::abs(number1 - number2) < tolerance;
    }

    /**
     * @brief 檢查是否為非數值(NaN)
     */
    inline bool IsNaN(float number) {
        return std::isnan(number);
    }

    /**
     * @brief 檢查是否為正無窮大
     */
    inline bool IsPositiveInfinity(float number) {
        return std::isinf(number) && std::signbit(number) == 0;
    }

    /**
     * @brief 檢查是否為負無窮大
     */
    inline bool IsNegativeInfinity(float number) {
        return std::isinf(number) && std::signbit(number) != 0;
    }

    // 數學運算 ----------------------------------------------------------------

    /**
     * @brief 四捨五入到指定小數位
     * @param decimalPlaces 小數位數（0-7）
     * @return 運算結果
     * @note 超出小數位精度時返回原始值
     */
    inline float RoundTo(float number, int decimalPlaces) {
        if (decimalPlaces < 0 || decimalPlaces > 7) return number;
        const float scale = std::powf(10.0f, decimalPlaces);
        return std::roundf(number * scale) / scale;
    }

    /**
     * @brief 向上取整
     */
    inline float Ceiling(float number) {
        return std::ceilf(number);
    }

    /**
     * @brief 向下取整
     */
    inline float Floor(float number) {
        return std::floorf(number);
    }

    /**
     * @brief 計算絕對值
     */
    inline float Abs(float number) {
        return std::abs(number);
    }

    /**
     * @brief 計算平方值
     */
    inline float Square(float number) {
        return number * number;
    }

    /**
     * @brief 計算平方根
     * @throws std::invalid_argument 當輸入為負數時拋出
     */
    inline float Sqrt(float number) {
        if (number < 0) {
            throw std::invalid_argument("不能對負數求平方根");
        }
        return std::sqrtf(number);
    }

    /**
     * @brief 計算冪次方
     */
    inline float Pow(float number, float power) {
        return std::powf(number, power);
    }

    // 格式轉換 ----------------------------------------------------------------

    /**
     * @brief 轉換為百分比字符串
     * @param decimalPlaces 小數位數（默認2位）
     * @return 格式化後的百分比字符串（如"75.50%"）
     */
    inline std::string ToPercentageString(float number, int decimalPlaces = 2) {
        std::ostringstream stream;
        stream.precision(decimalPlaces);
        stream << std::fixed << number * 100.0f << "%";
        return stream.str();
    }

    // 類型轉換 ----------------------------------------------------------------

    /**
     * @brief 轉換為雙精度浮點數
     */
    inline double ToDouble(float number) {
        return static_cast<double>(number);
    }

    /**
     * @brief 轉換為整數（直接截斷小數部分）
     */
    inline int ToInt(float number) {
        return static_cast<int>(number);
    }
}
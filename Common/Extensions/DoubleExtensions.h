#pragma once
#include <cmath>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <type_traits>

namespace DoubleUtils {
    // 數值檢驗與屬性判斷 ------------------------------------------------------

    /**
     * @brief 判斷雙精度浮點數是否為整數
     * @param number 要檢查的數值
     * @return 如果為整數返回true，否則返回false
     * @note 使用四捨五入後的差值與最小精度比較
     */
    inline bool IsInteger(double number) {
        return std::abs(number - std::round(number)) <
            std::numeric_limits<double>::epsilon();
    }

    /**
     * @brief 判斷兩個雙精度數是否近似相等
     * @param number1 第一個數
     * @param number2 第二個數
     * @param tolerance 容差值（默認為double精度）
     * @return 近似相等返回true，否則返回false
     */
    inline bool IsApproximatelyEqual(double number1, double number2,
        double tolerance = std::numeric_limits<double>::epsilon()) {
        return std::abs(number1 - number2) < tolerance;
    }

    /**
     * @brief 檢查是否為非數值(NaN)
     */
    inline bool IsNaN(double number) {
        return std::isnan(number);
    }

    /**
     * @brief 檢查是否為正無窮大
     */
    inline bool IsPositiveInfinity(double number) {
        return std::isinf(number) && std::signbit(number) == 0;
    }

    /**
     * @brief 檢查是否為負無窮大
     */
    inline bool IsNegativeInfinity(double number) {
        return std::isinf(number) && std::signbit(number) != 0;
    }

    // 數學運算 ----------------------------------------------------------------

    /**
     * @brief 四捨五入到指定小數位
     * @param decimalPlaces 小數位數（0-15）
     * @return 運算結果
     * @note 超出小數位精度時返回原始值
     */
    inline double RoundTo(double number, int decimalPlaces) {
        if (decimalPlaces < 0 || decimalPlaces > 15) return number;
        const double scale = std::pow(10.0, decimalPlaces);
        return std::round(number * scale) / scale;
    }

    /**
     * @brief 向上取整
     */
    inline double Ceiling(double number) {
        return std::ceil(number);
    }

    /**
     * @brief 向下取整
     */
    inline double Floor(double number) {
        return std::floor(number);
    }

    /**
     * @brief 計算絕對值
     */
    inline double Abs(double number) {
        return std::abs(number);
    }

    /**
     * @brief 計算平方值
     */
    inline double Square(double number) {
        return number * number;
    }

    /**
     * @brief 計算平方根
     * @throws std::invalid_argument 當輸入為負數時拋出
     */
    inline double Sqrt(double number) {
        if (number < 0) {
            throw std::invalid_argument("不能對負數求平方根");
        }
        return std::sqrt(number);
    }

    /**
     * @brief 計算冪次方
     */
    inline double Pow(double number, double power) {
        return std::pow(number, power);
    }

    // 格式轉換 ----------------------------------------------------------------

    /**
     * @brief 轉換為百分比字符串
     * @param decimalPlaces 小數位數（默認2位）
     * @return 格式化後的百分比字符串（如"75.50%"）
     */
    inline std::string ToPercentageString(double number, int decimalPlaces = 2) {
        std::ostringstream stream;
        stream.precision(decimalPlaces);
        stream << std::fixed << number * 100 << "%";
        return stream.str();
    }
}

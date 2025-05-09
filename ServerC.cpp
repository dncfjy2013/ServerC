// ServerC++.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "Common/Enums/ErrorCode.h"
#include "Common/Exceptions/ApiException.h"
#include "Common/Extensions/CharExtensions.h"
#include "Common/Extensions/DoubleExtensions.h"
#include "Common/Extensions/FloatExtensions.h"
#include "Common/Extensions/IntExtensions.h"
#include "Common/Extensions/StringExtensions.h"

int main()
{
    std::cout << "Hello World!\n" << ErrorCode::AccountLocked;

    char testChar = 'A';

    std::cout << std::boolalpha;
    std::cout << "IsUpper: " << CharUtils::IsUpperCase(testChar) << "\n";  // true
    std::cout << "IsLower: " << CharUtils::IsLowerCase(testChar) << "\n";  // false
    std::cout << "ToLower: " << CharUtils::ToLower(testChar) << "\n";      // 'a'
    std::cout << "IsHex: " << CharUtils::IsHexDigit('G') << "\n";          // false

    double testNum = 3.1415926535;

    std::cout << std::boolalpha;
    std::cout << "IsInteger: " << DoubleUtils::IsInteger(4.0) << "\n";    // true
    std::cout << "RoundTo: " << DoubleUtils::RoundTo(testNum, 3) << "\n"; // 3.142
    std::cout << "Percentage: " << DoubleUtils::ToPercentageString(0.755) << "\n"; // "75.50%"

    try {
        DoubleUtils::Sqrt(-1.0);
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Error: " << e.what() << "\n"; // 不能對負數求平方根
    }

    float test = 3.1415926f;

    std::cout << std::boolalpha;
    std::cout << "IsInteger: " << FloatUtils::IsInteger(4.0f) << "\n";    // true
    std::cout << "RoundTo: " << FloatUtils::RoundTo(test, 3) << "\n";  // 3.142
    std::cout << "Percentage: " << FloatUtils::ToPercentageString(0.755f) << "\n"; // "75.50%"

    try {
        FloatUtils::Sqrt(-1.0f);
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Error: " << e.what() << "\n"; // 不能對負數求平方根
    }

    std::cout << std::boolalpha;

    // 數值屬性
    std::cout << "IsEven(42): " << IntUtils::IsEven(42) << "\n";    // true
    std::cout << "IsPrime(17): " << IntUtils::IsPrime(17) << "\n";  // true

    // 數學運算
    std::cout << "Factorial(5): " << IntUtils::Factorial(5) << "\n";  // 120

    // 序列生成
    auto seq = IntUtils::To(5, 3);
    std::cout << "Sequence 5->3: ";
    for (int num : seq) std::cout << num << " ";  // 5 4 3

    // 數值轉換
    std::cout << "\nRoman(1999): " << IntUtils::ToRomanNumeral(1999);  // MCMXCIX
    std::cout << "\nBinary(-1): " << IntUtils::ToBinary(-1);           // 11111111...（32位全1）

    // 空值檢查
    std::cout << std::boolalpha;
    std::cout << "IsNullOrEmpty: " << StringUtils::IsNullOrEmpty("") << "\n";      // true

    // 格式轉換
    std::cout << "CamelCase: " << StringUtils::ToCamelCase("helloWorld") << "\n";   // helloWorld

    // 安全處理
    std::cout << "MD5: " << StringUtils::ToMD5("password") << "\n";                 // 5f4dcc3b5aa765d61d8327deb882cf99

    // 驗證方法
    std::cout << "ValidEmail: " << StringUtils::IsValidEmail("test@example.com") << "\n"; // true

    // 字符串處理
    std::cout << "RandomString: " << StringUtils::RandomString(10) << "\n";          // 隨機字符串

    // 編碼轉換
    std::cout << "Base64: " << StringUtils::ToBase64("test") << "\n";                // dGVzdA==

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

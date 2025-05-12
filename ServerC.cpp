// ServerC++.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "Common/Enums/ErrorCode.hpp"
#include "Common/Exceptions/ApiException.hpp"
#include "Common/Extensions/CharExtensions.hpp"
#include "Common/Extensions/DoubleExtensions.hpp"
#include "Common/Extensions/FloatExtensions.hpp"
#include "Common/Extensions/IntExtensions.hpp"
#include "Common/Extensions/StringExtensions.hpp"
#include "Common/Helpers/DateTimeHelper.hpp"
#include "Utils/StateMachine.hpp"

// 定义状态类型和键类型
using State = std::string;
using Key = std::string;

// 前置状态转换回调函数
void beforeTransition(const Key& key, const State& from, const State& to) {
    std::cout << "Before transition for key: " << key
        << ", from: " << from << ", to: " << to << std::endl;
}

// 后置状态转换回调函数
void afterTransition(const Key& key, const State& from, const State& to) {
    std::cout << "After transition for key: " << key
        << ", from: " << from << ", to: " << to << std::endl;
}

// 状态转换失败回调函数
void transitionFailed(const Key& key, const State& from, const State& to, const std::exception& ex) {
    std::cout << "Transition failed for key: " << key
        << ", from: " << from << ", to: " << to
        << ", error: " << ex.what() << std::endl;
}

// 将时间点转换为字符串以便输出
std::string timePointToString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    char buffer[64];
    std::tm tm_info;

#ifdef _WIN32
    // Windows 平台使用 localtime_s
    localtime_s(&tm_info, &t);
#else
    // POSIX 平台使用 localtime_r
    localtime_r(&t, &tm_info);
#endif

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info);
    return std::string(buffer);
}

int statetest() {
    // 创建状态机实例
    StateMachine<Key, State> stateMachine;

    // 设置回调函数
    stateMachine._onBeforeTransition = beforeTransition;
    stateMachine._onAfterTransition = afterTransition;
    stateMachine._onTransitionFailed = transitionFailed;

    // 定义状态机键和初始状态
    Key key = "MyStateMachine";
    State initialState = "Idle";

    // 初始化状态机
    stateMachine.InitializeState(key, initialState);
    std::cout << "State machine initialized with key: " << key
        << ", initial state: " << initialState << std::endl;

    // 添加合法的状态转换规则
    stateMachine.AddTransition("Idle", "Processing");
    stateMachine.AddTransition("Processing", "Completed");
    stateMachine.AddTransition("Processing", "Failed");
    stateMachine.AddTransition("Completed", "Idle");
    stateMachine.AddTransition("Failed", "Idle");

    // 定义状态转换动作
    auto processingAction = [](const Key& k, const State& from, const State& to) {
        std::cout << "Performing action for transition: " << from << " -> " << to
            << " (Key: " << k << ")" << std::endl;
        // 模拟处理过程
        std::this_thread::sleep_for(std::chrono::seconds(2));
        };

    // 从 Idle 转换到 Processing
    bool success = stateMachine.Transition(
        key, "Processing", processingAction, "User requested processing");

    if (success) {
        std::cout << "Successfully transitioned to Processing state" << std::endl;
    }
    else {
        std::cout << "Failed to transition to Processing state" << std::endl;
    }

    // 设置超时（如果在 3 秒内没有进一步操作，自动回退到 Idle）
    stateMachine.SetTimeout(key, std::chrono::seconds(3), "Idle");
    std::cout << "Timeout set for Processing state: 3 seconds" << std::endl;

    // 模拟等待超时
    std::cout << "Waiting for timeout..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // 尝试从 Processing 转换到 Completed（应该失败，因为已超时回退到 Idle）
    success = stateMachine.Transition(
        key, "Completed", processingAction, "Attempting to complete processing");

    if (success) {
        std::cout << "Successfully transitioned to Completed state" << std::endl;
    }
    else {
        std::cout << "Failed to transition to Completed state" << std::endl;
    }

    // 再次从 Idle 转换到 Processing
    success = stateMachine.Transition(
        key, "Processing", processingAction, "Retry processing");

    if (success) {
        std::cout << "Successfully transitioned to Processing state again" << std::endl;

        // 这次不等待超时，直接完成处理
        std::this_thread::sleep_for(std::chrono::seconds(1));

        success = stateMachine.Transition(
            key, "Completed", processingAction, "Processing finished");

        if (success) {
            std::cout << "Successfully transitioned to Completed state" << std::endl;

            // 回到初始状态
            success = stateMachine.Transition(
                key, "Idle", [](auto, auto, auto) {}, "Return to idle");

            if (success) {
                std::cout << "Successfully transitioned back to Idle state" << std::endl;
            }
        }
    }

    // 获取状态变更历史并打印
    auto history = stateMachine.GetStateHistory(key);
    std::cout << "\nState History for key " << key << ":\n";
    for (const auto& entry : history) {
        const State& state = std::get<0>(entry);
        const std::chrono::system_clock::time_point& timestamp = std::get<1>(entry);
        const std::string& reason = std::get<2>(entry);

        std::cout << "- State: " << state
            << ", Time: " << timePointToString(timestamp)
            << ", Reason: " << reason << std::endl;
    }

    // 获取审计日志并打印
    auto auditLogs = stateMachine.GetAuditLogs();
    std::cout << "\nAudit Logs:\n";
    for (const auto& log : auditLogs) {
        std::cout << "- Time: " << timePointToString(log.timestamp)
            << ", Key: " << log.key
            << ", From: " << log.fromState
            << ", To: " << log.toState
            << ", Success: " << (log.success ? "Yes" : "No")
            << ", Error: " << (log.error.empty() ? "None" : log.error) << std::endl;
    }

    return 0;
}

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

    // 創建測試時間點
    auto now = std::chrono::system_clock::now();
    auto nextMonth = DateTimeUtils::AddMonthsSafe(now, 1);

    // 計算月差
    std::cout << "Months between: "
        << DateTimeUtils::MonthsBetween(now, nextMonth) << "\n";  // 1

    // 格式化輸出
    std::cout << "Custom format: "
        << DateTimeUtils::ToCustomFormat(now, "%Y-%m-%d %H:%M:%S") << "\n";

    // 季度計算
    auto firstDayOfQuarter = DateTimeUtils::GetFirstDayOfQuarter(now);
    std::cout << "First day of quarter: "
        << DateTimeUtils::ToCustomFormat(firstDayOfQuarter, "%Y-%m-%d") << "\n";

    int a = statetest();
    
    int b;
    std::cin >> b;
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

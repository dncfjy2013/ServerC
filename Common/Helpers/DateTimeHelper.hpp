#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <array>

namespace DateTimeUtils {
    // 工具函数：将std::chrono时间点转换为std::tm（本地时间）
    /// @brief 将时间点转换为本地时间的tm结构
    /// @param tp 输入的system_clock时间点
    /// @return 转换后的tm结构体（表示本地时间）
    /// @note 使用std::localtime进行转换，结果受系统时区设置影响
    inline std::tm to_tm(const std::chrono::system_clock::time_point& tp) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm = *std::localtime(&t);
        return tm;
    }

    // 工具函数：将std::tm转换为std::chrono时间点
    /// @brief 将tm结构转换为system_clock时间点
    /// @param tm 输入的tm结构体（本地时间）
    /// @return 对应的时间点
    /// @note 使用std::mktime进行转换，会自动标准化tm字段（处理溢出）
    inline std::chrono::system_clock::time_point from_tm(const std::tm& tm) {
        std::time_t t = std::mktime(const_cast<std::tm*>(&tm));
        return std::chrono::system_clock::from_time_t(t);
    }

    // 计算两个日期之间的天数差
    /// @brief 计算两个时间点之间的自然天数差
    /// @param start 起始时间点
    /// @param end 结束时间点
    /// @return 两个日期之间的完整天数差（end - start）
    /// @note 基于24小时制计算，可能受夏令时影响
    inline int DaysBetween(const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        auto diff = end - start;
        return std::chrono::duration_cast<std::chrono::hours>(diff).count() / 24;
    }

    // 获取星期字符串
    /// @brief 获取指定日期的星期名称（英文）
    /// @param date 输入时间点
    /// @return 星期全称（如"Monday"），tm_wday=0对应Sunday
    /// @note 使用静态数组存储星期名称，索引与tm_wday对应
    inline std::string GetDayOfWeek(const std::chrono::system_clock::time_point& date) {
        static const std::array<std::string, 7> weekDays = {
            "Sunday", "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturday"
        };
        auto tm = to_tm(date);
        return weekDays[tm.tm_wday];
    }

    // 自定义格式转换
    /// @brief 将时间点转换为自定义格式字符串
    /// @param date 输入时间点
    /// @param format 格式字符串（支持%Y年%m月%d日等格式）
    /// @return 格式化后的时间字符串
    /// @note 支持格式符：
    ///       - %Y 四位年份
    ///       - %m 两位月份（01-12）
    ///       - %d 两位日期（01-31）
    ///       - %H 24小时制小时（00-23）
    ///       - %M 分钟（00-59）
    ///       - %S 秒（00-59）
    inline std::string ToCustomFormat(const std::chrono::system_clock::time_point& date,
        const std::string& format) {
        std::tm tm = to_tm(date);
        std::stringstream ss;

        for (size_t i = 0; i < format.size(); ++i) {
            if (format[i] == '%') {
                switch (format[++i]) {
                case 'Y': ss << 1900 + tm.tm_year; break;
                case 'm': ss << std::setw(2) << std::setfill('0') << tm.tm_mon + 1; break;
                case 'd': ss << std::setw(2) << std::setfill('0') << tm.tm_mday; break;
                case 'H': ss << std::setw(2) << std::setfill('0') << tm.tm_hour; break;
                case 'M': ss << std::setw(2) << std::setfill('0') << tm.tm_min; break;
                case 'S': ss << std::setw(2) << std::setfill('0') << tm.tm_sec; break;
                default:  ss << '%' << format[i];
                }
            }
            else {
                ss << format[i];
            }
        }
        return ss.str();
    }

    // 判断闰年
    /// @brief 判断指定年份是否为闰年
    /// @param year 四位年份（如2024）
    /// @return 闰年返回true，否则返回false
    /// @note 闰年规则：能被4整除但不能被100整除，或能被400整除
    inline bool IsLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    // 月份第一天
    /// @brief 获取指定日期所在月份的第一天
    /// @param date 输入时间点
    /// @return 当月第一天的时间点（00:00:00）
    inline std::chrono::system_clock::time_point GetFirstDayOfMonth(
        const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        tm.tm_mday = 1;
        return from_tm(tm);
    }

    // 月份最后一天
    /// @brief 获取指定日期所在月份的最后一天
    /// @param date 输入时间点
    /// @return 当月最后一天的时间点（23:59:59）
    /// @note 通过跳转到下个月第一天再减1天实现
    inline std::chrono::system_clock::time_point GetLastDayOfMonth(
        const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        tm.tm_mday = 1;
        tm.tm_mon += 1;
        return from_tm(tm) - std::chrono::hours(24);
    }

    // 周数差
    /// @brief 计算两个时间点之间的完整周数差
    /// @param start 起始时间点
    /// @param end 结束时间点
    /// @return 完整周数差（向下取整）
    inline int WeeksBetween(const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        return DaysBetween(start, end) / 7;
    }

    // 月数差
    /// @brief 计算两个时间点之间的月份差
    /// @param start 起始时间点
    /// @param end 结束时间点
    /// @return 月份差（基于年月计算，非实际天数）
    /// @note 例如：2024-01-15到2024-03-10返回2个月
    inline int MonthsBetween(const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        auto startTm = to_tm(start);
        auto endTm = to_tm(end);
        return (endTm.tm_year - startTm.tm_year) * 12 + (endTm.tm_mon - startTm.tm_mon);
    }

    // 年数差
    /// @brief 计算两个时间点之间的完整年份差
    /// @param start 起始时间点
    /// @param end 结束时间点
    /// @return 完整年份差（考虑未满年的情况）
    /// @note 例如：2020-03-15到2023-02-28返回2年
    inline int YearsBetween(const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        auto startTm = to_tm(start);
        auto endTm = to_tm(end);
        int years = endTm.tm_year - startTm.tm_year;
        if (endTm.tm_mon < startTm.tm_mon ||
            (endTm.tm_mon == startTm.tm_mon && endTm.tm_mday < startTm.tm_mday)) {
            years--;
        }
        return years;
    }

    // 周第一天（默认周一）
    /// @brief 获取指定日期所在周的第一天
    /// @param date 输入时间点
    /// @param firstDay 周起始日（0=周日，1=周一，默认1）
    /// @return 周第一天的时间点（00:00:00）
    /// @note 使用tm_wday计算，受系统时区影响
    inline std::chrono::system_clock::time_point GetFirstDayOfWeek(
        const std::chrono::system_clock::time_point& date,
        int firstDay = 1) { // 0=Sunday, 1=Monday
        auto tm = to_tm(date);
        int diff = (tm.tm_wday - firstDay + 7) % 7;
        return from_tm(tm) - std::chrono::hours(24 * diff);
    }

    // 安全添加月份
    /// @brief 安全地向时间点添加指定月份数
    /// @param date 输入时间点
    /// @param months 要添加的月份数（可正可负）
    /// @return 添加后的新时间点
    /// @note 自动处理月末日期溢出（如1月31日+1月=2月28日/29日）
    inline std::chrono::system_clock::time_point AddMonthsSafe(
        const std::chrono::system_clock::time_point& date,
        int months) {
        auto tm = to_tm(date);
        tm.tm_mon += months;
        return from_tm(tm); // mktime会自动标准化tm字段
    }

    // 工作日判断
    /// @brief 判断指定日期是否为工作日（周一至周五）
    /// @param date 输入时间点
    /// @return 是工作日返回true，否则返回false
    /// @note tm_wday范围：0=周日，1-5=周一到周五，6=周六
    inline bool IsWeekday(const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        return tm.tm_wday != 0 && tm.tm_wday != 6;
    }

    // 季度计算
    /// @brief 获取指定日期所在季度的第一天
    /// @param date 输入时间点
    /// @return 季度第一天的时间点（00:00:00）
    /// @note 季度划分：1-3月为Q1，4-6月为Q2，依此类推
    inline std::chrono::system_clock::time_point GetFirstDayOfQuarter(
        const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        tm.tm_mon = (tm.tm_mon / 3) * 3;
        tm.tm_mday = 1;
        return from_tm(tm);
    }

    // Unix时间戳转换
    /// @brief 从Unix时间戳创建时间点
    /// @param timestamp Unix时间戳（秒级，自1970-01-01 UTC）
    /// @return 对应的时间点
    /// @note 适用于32位和64位系统的时间戳转换
    inline std::chrono::system_clock::time_point FromUnixTimestamp(long long timestamp) {
        return std::chrono::system_clock::from_time_t(timestamp);
    }

    /// @brief 将时间点转换为Unix时间戳
    /// @param date 输入时间点
    /// @return Unix时间戳（秒级）
    /// @note 返回的timestamp_t与系统时钟精度相关
    inline long long ToUnixTimestamp(
        const std::chrono::system_clock::time_point& date) {
        return std::chrono::system_clock::to_time_t(date);
    }
}
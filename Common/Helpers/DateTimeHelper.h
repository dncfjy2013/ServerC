#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <array>

namespace DateTimeUtils {
    // ���ߺ�������std::chronoʱ���ת��Ϊstd::tm������ʱ�䣩
    /// @brief ��ʱ���ת��Ϊ����ʱ���tm�ṹ
    /// @param tp �����system_clockʱ���
    /// @return ת�����tm�ṹ�壨��ʾ����ʱ�䣩
    /// @note ʹ��std::localtime����ת���������ϵͳʱ������Ӱ��
    inline std::tm to_tm(const std::chrono::system_clock::time_point& tp) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm = *std::localtime(&t);
        return tm;
    }

    // ���ߺ�������std::tmת��Ϊstd::chronoʱ���
    /// @brief ��tm�ṹת��Ϊsystem_clockʱ���
    /// @param tm �����tm�ṹ�壨����ʱ�䣩
    /// @return ��Ӧ��ʱ���
    /// @note ʹ��std::mktime����ת�������Զ���׼��tm�ֶΣ����������
    inline std::chrono::system_clock::time_point from_tm(const std::tm& tm) {
        std::time_t t = std::mktime(const_cast<std::tm*>(&tm));
        return std::chrono::system_clock::from_time_t(t);
    }

    // ������������֮���������
    /// @brief ��������ʱ���֮�����Ȼ������
    /// @param start ��ʼʱ���
    /// @param end ����ʱ���
    /// @return ��������֮������������end - start��
    /// @note ����24Сʱ�Ƽ��㣬����������ʱӰ��
    inline int DaysBetween(const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        auto diff = end - start;
        return std::chrono::duration_cast<std::chrono::hours>(diff).count() / 24;
    }

    // ��ȡ�����ַ���
    /// @brief ��ȡָ�����ڵ��������ƣ�Ӣ�ģ�
    /// @param date ����ʱ���
    /// @return ����ȫ�ƣ���"Monday"����tm_wday=0��ӦSunday
    /// @note ʹ�þ�̬����洢�������ƣ�������tm_wday��Ӧ
    inline std::string GetDayOfWeek(const std::chrono::system_clock::time_point& date) {
        static const std::array<std::string, 7> weekDays = {
            "Sunday", "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturday"
        };
        auto tm = to_tm(date);
        return weekDays[tm.tm_wday];
    }

    // �Զ����ʽת��
    /// @brief ��ʱ���ת��Ϊ�Զ����ʽ�ַ���
    /// @param date ����ʱ���
    /// @param format ��ʽ�ַ�����֧��%Y��%m��%d�յȸ�ʽ��
    /// @return ��ʽ�����ʱ���ַ���
    /// @note ֧�ָ�ʽ����
    ///       - %Y ��λ���
    ///       - %m ��λ�·ݣ�01-12��
    ///       - %d ��λ���ڣ�01-31��
    ///       - %H 24Сʱ��Сʱ��00-23��
    ///       - %M ���ӣ�00-59��
    ///       - %S �루00-59��
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

    // �ж�����
    /// @brief �ж�ָ������Ƿ�Ϊ����
    /// @param year ��λ��ݣ���2024��
    /// @return ���귵��true�����򷵻�false
    /// @note ��������ܱ�4���������ܱ�100���������ܱ�400����
    inline bool IsLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    // �·ݵ�һ��
    /// @brief ��ȡָ�����������·ݵĵ�һ��
    /// @param date ����ʱ���
    /// @return ���µ�һ���ʱ��㣨00:00:00��
    inline std::chrono::system_clock::time_point GetFirstDayOfMonth(
        const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        tm.tm_mday = 1;
        return from_tm(tm);
    }

    // �·����һ��
    /// @brief ��ȡָ�����������·ݵ����һ��
    /// @param date ����ʱ���
    /// @return �������һ���ʱ��㣨23:59:59��
    /// @note ͨ����ת���¸��µ�һ���ټ�1��ʵ��
    inline std::chrono::system_clock::time_point GetLastDayOfMonth(
        const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        tm.tm_mday = 1;
        tm.tm_mon += 1;
        return from_tm(tm) - std::chrono::hours(24);
    }

    // ������
    /// @brief ��������ʱ���֮�������������
    /// @param start ��ʼʱ���
    /// @param end ����ʱ���
    /// @return �������������ȡ����
    inline int WeeksBetween(const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        return DaysBetween(start, end) / 7;
    }

    // ������
    /// @brief ��������ʱ���֮����·ݲ�
    /// @param start ��ʼʱ���
    /// @param end ����ʱ���
    /// @return �·ݲ�������¼��㣬��ʵ��������
    /// @note ���磺2024-01-15��2024-03-10����2����
    inline int MonthsBetween(const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) {
        auto startTm = to_tm(start);
        auto endTm = to_tm(end);
        return (endTm.tm_year - startTm.tm_year) * 12 + (endTm.tm_mon - startTm.tm_mon);
    }

    // ������
    /// @brief ��������ʱ���֮���������ݲ�
    /// @param start ��ʼʱ���
    /// @param end ����ʱ���
    /// @return ������ݲ����δ����������
    /// @note ���磺2020-03-15��2023-02-28����2��
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

    // �ܵ�һ�죨Ĭ����һ��
    /// @brief ��ȡָ�����������ܵĵ�һ��
    /// @param date ����ʱ���
    /// @param firstDay ����ʼ�գ�0=���գ�1=��һ��Ĭ��1��
    /// @return �ܵ�һ���ʱ��㣨00:00:00��
    /// @note ʹ��tm_wday���㣬��ϵͳʱ��Ӱ��
    inline std::chrono::system_clock::time_point GetFirstDayOfWeek(
        const std::chrono::system_clock::time_point& date,
        int firstDay = 1) { // 0=Sunday, 1=Monday
        auto tm = to_tm(date);
        int diff = (tm.tm_wday - firstDay + 7) % 7;
        return from_tm(tm) - std::chrono::hours(24 * diff);
    }

    // ��ȫ����·�
    /// @brief ��ȫ����ʱ������ָ���·���
    /// @param date ����ʱ���
    /// @param months Ҫ��ӵ��·����������ɸ���
    /// @return ��Ӻ����ʱ���
    /// @note �Զ�������ĩ�����������1��31��+1��=2��28��/29�գ�
    inline std::chrono::system_clock::time_point AddMonthsSafe(
        const std::chrono::system_clock::time_point& date,
        int months) {
        auto tm = to_tm(date);
        tm.tm_mon += months;
        return from_tm(tm); // mktime���Զ���׼��tm�ֶ�
    }

    // �������ж�
    /// @brief �ж�ָ�������Ƿ�Ϊ�����գ���һ�����壩
    /// @param date ����ʱ���
    /// @return �ǹ����շ���true�����򷵻�false
    /// @note tm_wday��Χ��0=���գ�1-5=��һ�����壬6=����
    inline bool IsWeekday(const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        return tm.tm_wday != 0 && tm.tm_wday != 6;
    }

    // ���ȼ���
    /// @brief ��ȡָ���������ڼ��ȵĵ�һ��
    /// @param date ����ʱ���
    /// @return ���ȵ�һ���ʱ��㣨00:00:00��
    /// @note ���Ȼ��֣�1-3��ΪQ1��4-6��ΪQ2����������
    inline std::chrono::system_clock::time_point GetFirstDayOfQuarter(
        const std::chrono::system_clock::time_point& date) {
        auto tm = to_tm(date);
        tm.tm_mon = (tm.tm_mon / 3) * 3;
        tm.tm_mday = 1;
        return from_tm(tm);
    }

    // Unixʱ���ת��
    /// @brief ��Unixʱ�������ʱ���
    /// @param timestamp Unixʱ������뼶����1970-01-01 UTC��
    /// @return ��Ӧ��ʱ���
    /// @note ������32λ��64λϵͳ��ʱ���ת��
    inline std::chrono::system_clock::time_point FromUnixTimestamp(long long timestamp) {
        return std::chrono::system_clock::from_time_t(timestamp);
    }

    /// @brief ��ʱ���ת��ΪUnixʱ���
    /// @param date ����ʱ���
    /// @return Unixʱ������뼶��
    /// @note ���ص�timestamp_t��ϵͳʱ�Ӿ������
    inline long long ToUnixTimestamp(
        const std::chrono::system_clock::time_point& date) {
        return std::chrono::system_clock::to_time_t(date);
    }
}
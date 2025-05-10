#pragma once
#include <cctype>
#include <string>
#include <regex>
#include <unordered_set>

namespace CharUtils {
    // �ַ�����Д�ϵ�� ----------------------------------------------------------

    /**
     * @brief �Д��ַ��Ƿ�����ĸ��A-Z��
     * @param c Ҫ�z����ַ�
     * @return ����Ǵ���ĸ����true����t����false
     * @note ʹ��std::isupper�K�����D�Q��unsigned char
     *       �Ա���ĳЩ���F�еķ�̖�Uչ���}
     */
    inline bool IsUpperCase(char c) {
        return std::isupper(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ��С����ĸ��a-z��
     * @param c Ҫ�z����ַ�
     * @return �����С����ĸ����true����t����false
     */
    inline bool IsLowerCase(char c) {
        return std::islower(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ��ʮ�M�Ɣ��֣�0-9��
     * @param c Ҫ�z����ַ�
     * @return ����ǔ��ַ���true����t����false
     */
    inline bool IsDigit(char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ����ĸ��������С����
     * @param c Ҫ�z����ַ�
     * @return �������ĸ����true����t����false
     */
    inline bool IsLetter(char c) {
        return std::isalpha(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ����ĸ����
     * @param c Ҫ�z����ַ�
     * @return �������ĸ/���ַ���true����t����false
     */
    inline bool IsLetterOrDigit(char c) {
        return std::isalnum(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ��հ��ַ�
     * @param c Ҫ�z����ַ�
     * @return ����ǿհ��ַ�����true����t����false
     * @note �����ո񡢓Q�з����Ʊ���ȣ���C++�^���O����
     */
    inline bool IsWhiteSpace(char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ����c��̖
     * @param c Ҫ�z����ַ�
     * @return ����ǘ��c����true����t����false
     * @note ��C++�^���O����ͨ������!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
     */
    inline bool IsPunctuation(char c) {
        return std::ispunct(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ������ַ�
     * @param c Ҫ�z����ַ�
     * @return ����ǿ����ַ�����true����t����false
     * @note �����ַ�����ASCII 0-31��127��DEL��
     */
    inline bool IsControl(char c) {
        return std::iscntrl(static_cast<unsigned char>(c));
    }

    /**
     * @brief �Д��ַ��Ƿ���̖�ַ�������ĸ���ֵĘ��c��
     * @param c Ҫ�z����ַ�
     * @return ����Ƿ�̖����true����t����false
     * @note ���F�˳�Ҋ��̖�ļ��ϣ��ɰ���Uչsymbols����
     */
    inline bool IsSymbol(char c) {
        const std::unordered_set<char> symbols = {
            '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
            ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~'
        };
        return symbols.count(c) > 0;
    }

    // �ַ��D�Qϵ�� ----------------------------------------------------------

    /**
     * @brief ���ַ��D�Q�����ʽ
     * @param c Ҫ�D�Q���ַ�
     * @return �D�Q��Ĵ��ַ������ԭ�ַ�������ĸ�tֱ�ӷ��أ�
     */
    inline char ToUpper(char c) {
        return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    /**
     * @brief ���ַ��D�Q��С����ʽ
     * @param c Ҫ�D�Q���ַ�
     * @return �D�Q���С���ַ������ԭ�ַ�������ĸ�tֱ�ӷ��أ�
     */
    inline char ToLower(char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    // �ַ������Д�ϵ�� ------------------------------------------------------

    /**
     * @brief �@ȡ�ַ���ASCII�aֵ
     * @param c Ҫ�D�Q���ַ�
     * @return �ַ���ASCII�aֵ��0-255��
     */
    inline int ToAsciiCode(char c) {
        return static_cast<int>(c);
    }

    /**
     * @brief �Д��ַ��Ƿ��Ԫ����ĸ�����^�ִ�С����
     * @param c Ҫ�z����ַ�
     * @return �����Ԫ����a/e/i/o/u������true����t����false
     */
    inline bool IsVowel(char c) {
        const char lower = ToLower(c);
        return lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u';
    }

    /**
     * @brief �Д��ַ��Ƿ��ʮ���M�Ɣ��֣�0-9, a-f, A-F��
     * @param c Ҫ�z����ַ�
     * @return �����ʮ���M�Ɣ��ַ���true����t����false
     * @note �������F�������t���_ʽ����Ч
     */
    inline bool IsHexDigit(char c) {
        return (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F');
    }
}

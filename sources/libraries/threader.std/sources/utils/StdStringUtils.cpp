#include "StdStringUtils.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <algorithm>

namespace Threader {

namespace Utils {

std::string StdStringUtils::toUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

std::string StdStringUtils::toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

bool StdStringUtils::endsWith(const std::string &s, const std::string &end)
{
    unsigned endIndex = s.length() - end.length();
    return s.compare(endIndex, end.length(), end) == 0;
}

#ifdef WIN32

std::string StdStringUtils::ConvertCP1251ToUTF8(const std::string &str)
{
    int len = MultiByteToWideChar(1251, 0, str.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len];
    MultiByteToWideChar(1251, 0, str.c_str(), -1, wstr, len);

    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, 0, 0);
    char* utf8 = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, 0, 0);

    std::string result(utf8);
    delete[] wstr;
    delete[] utf8;

    return result;
}

std::string StdStringUtils::ConvertUTF8ToCP1251(const std::string &str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr, len);

    len = WideCharToMultiByte(1251, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* cp1251 = new char[len];
    WideCharToMultiByte(1251, 0, wstr, -1, cp1251, len, NULL, NULL);

    std::string result(cp1251);
    delete[] wstr;
    delete[] cp1251;

    return result;
}

#endif

}}

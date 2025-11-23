#include "StdStringUtils.h"
#include <cstring>
#include <sstream>

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

bool StdStringUtils::startWith(const std::string &s, const std::string &start)
{
    return s.compare(0, start.length(), start) == 0;
}

bool StdStringUtils::endsWith(const std::string &s, const std::string &end)
{
    unsigned endIndex = s.length() - end.length();
    return s.compare(endIndex, end.length(), end) == 0;
}

StdStringVector StdStringUtils::split(const std::string &str,
                                      const char delimiter,
                                      const bool keepEmptyParts)
{
    StdStringVector result;
    std::string token;
    std::istringstream stream(str);

    while (std::getline(stream, token, delimiter))
    {
        if (keepEmptyParts || !token.empty())
            result.push_back(token);
    }

    return result;
}

std::string StdStringUtils::section(const std::string &str,
                                    const char delimiter,
                                    const int number,
                                    const bool keepEmptyParts)
{
    auto words = split(str, delimiter, keepEmptyParts);
    if (words.size() <= number)
        return std::string();

    return words[number];
}

std::string StdStringUtils::ltrim(const std::string &s)
{
    if (s.empty())
        return s;
    auto result = s;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch)
    {
        return !std::isspace(ch);
    }));
    return result;
}

std::string StdStringUtils::rtrim(const std::string &s)
{
    if (s.empty())
        return s;
    auto result = s;
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch)
    {
        return !std::isspace(ch);
    }).base(), result.end());
    return result;
}

std::string StdStringUtils::trim(const std::string &s)
{
    return ltrim(rtrim(s));
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

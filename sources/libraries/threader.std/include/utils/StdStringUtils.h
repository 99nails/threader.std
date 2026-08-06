#pragma once

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <list>
#include <vector>
#include <stdint.h>
#include <string>

namespace Threader {

namespace Utils {

using StdStringList = std::list<std::string>;

using StdStringVector = std::vector<std::string>;

class StdStringUtils
{
public:
    static std::string toUpper(std::string s);

    static std::string toLower(std::string s);

    static bool startWith(const std::string &s,
                          const std::string &start);

    static bool endsWith(const std::string &s,
                         const std::string &end);

    static StdStringVector split(const std::string &str,
                                 const char delimiter,
                                 const bool keepEmptyParts);

    static std::string section(const std::string &str,
                               const char delimiter,
                               const int number,
                               const bool keepEmptyParts);

    static std::string ltrim(const std::string &s);

    static std::string rtrim(const std::string &s);

    static std::string trim(const std::string &s);
#ifdef WIN32
    static std::string ConvertCP1251ToUTF8(const std::string &str);

    static std::string ConvertUTF8ToCP1251(const std::string &str);
#endif
};

template <size_t STACK_BUFFER_SIZE=256>
static std::string format(const char * format, va_list &vl_original)
{
    std::string result;
    char stackBuffer[STACK_BUFFER_SIZE];
    char *heapBuffer = nullptr;
    char *choosenBuffer = stackBuffer;

    va_list valueListWorking;
    va_list valueListWorking2;

    va_copy(valueListWorking, vl_original);
    va_copy(valueListWorking2, vl_original);

    size_t requiredSize = vsnprintf(stackBuffer,
                                    STACK_BUFFER_SIZE,
                                    format,
                                    valueListWorking);

    if (requiredSize >= STACK_BUFFER_SIZE)
    {
        uint32_t heapBufferSize = requiredSize + 1;
        heapBuffer = new char[heapBufferSize];
        vsnprintf(heapBuffer, heapBufferSize, format, valueListWorking2);
        choosenBuffer = heapBuffer;
    }

    va_end(valueListWorking);
    va_end(valueListWorking2);

    result.assign(choosenBuffer);

    if (heapBuffer)
        delete[] heapBuffer;

    return result;
}

std::string format(const char * formatString, ...);

template <typename T>
bool vtonum(const std::string &view, T& value)
{
    if (view.empty()) {
        return false;
    }

    const char* first = view.data();
    const char* last = view.data() + view.length();

    std::from_chars_result res = std::from_chars(first, last, value);

    if (res.ec != std::errc()) {
        return false;
    }

    if (res.ptr != last) {
        return false;
    }

    return true;
}

// TODO Обработать ошибки
template <>
inline bool vtonum<double>(const std::string &view, double &value)
{
    std::string s(view);
    std::replace(s.begin(), s.end(), ',', '.');
    value = std::strtod(s.c_str(), nullptr);
    return true;
}

template <>
inline bool vtonum<bool>(const std::string &view, bool &value)
{
    value = (view == "true" || view == "1");
    return true;
}

}}


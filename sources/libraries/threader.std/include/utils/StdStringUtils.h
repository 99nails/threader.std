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

}}

template <size_t STACK_BUFFER_SIZE=256>
static std::string format(const char * format, ...)
{
    std::string result;
    char stack_buffer[STACK_BUFFER_SIZE];
    char *heap_buffer = nullptr;
    char *choosen_buffer = stack_buffer;

    va_list vl_original;
    va_list vl_working;
    va_list vl_working_2;

    va_start(vl_original, format);
    va_copy(vl_working, vl_original);
    va_copy(vl_working_2, vl_original);

    size_t chars_that_would_be_printed = vsnprintf(stack_buffer,
                                                   STACK_BUFFER_SIZE,
                                                   format,
                                                   vl_working);

    if (chars_that_would_be_printed >= STACK_BUFFER_SIZE)
    {
        uint32_t heap_buffer_size = chars_that_would_be_printed + 1;
        heap_buffer = new char[heap_buffer_size];
        vsnprintf(heap_buffer, heap_buffer_size, format, vl_working_2);
        choosen_buffer = heap_buffer;
    }

    va_end(vl_working);
    va_end(vl_working_2);
    va_end(vl_original);

    result.assign(choosen_buffer);

    if (heap_buffer)
        delete[] heap_buffer;

    return result;
}

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

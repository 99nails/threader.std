#include "DateTimeUtils.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/time.h>

namespace Threader::Utils {

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

#ifdef WIN32
GetTickCount64Prototype* DateTimeUtils::_getTickCount = nullptr;
#endif

UnixTimeMSecs DateTimeUtils::msecsSinceEpochLocal()
{
    auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return millisec_since_epoch.count();
}

std::string DateTimeUtils::msecsSinceEpochToStdString(const UnixTimeMSecs &msecsSinceEpoch,
                                                      const bool addMsecs,
                                                      const bool isLocal,
                                                      const std::string format)
{
    std::time_t temp = msecsSinceEpoch / 1000;
    int msecs = msecsSinceEpoch % 1000;

    auto time = (isLocal) ? std::localtime(&temp) : std::gmtime(&temp);

    std::stringstream ss;
    ss << std::put_time(time, format.c_str());
    if (addMsecs)
    {
        std::string msecsString = std::to_string(msecs);
        while (msecsString.length() < 3)
            msecsString = '0' + msecsString;
        ss << '.' << msecsString;
    }
    auto result = ss.str();
    return result;
}

UnixTimeMSecs DateTimeUtils::StdStringToMsecsSinceEpoch(const std::string &from, const std::string &format)
{
    std::istringstream ss(from);
    std::tm t{};
    ss >> std::get_time(&t, format.c_str());
    if (ss.fail())
        return 0;

    std::time_t timeSinceEpoch = mktime(&t);
    if (timeSinceEpoch == -1) {
        std::cerr << "Failed to convert to time_t\n";
        return 0;
    }

    return timeSinceEpoch * 1000;
}


bool DateTimeUtils::setCurrentDateTime(const UnixTimeMSecs &dateTime)
{
#ifdef Q_OS_WIN
    SYSTEMTIME systemTime;
    memset(&systemTime, 0, sizeof(systemTime));

    QDate date = dateTime.date();
    QTime time = dateTime.time();

    systemTime.wYear = date.year();
    systemTime.wMonth = date.month();
    systemTime.wDay = date.day();
    systemTime.wDayOfWeek = date.dayOfWeek() % 7;
    systemTime.wHour = time.hour();
    systemTime.wMinute = time.minute();
    systemTime.wSecond = time.second();
    systemTime.wMilliseconds = time.msec();

    auto result = SetLocalTime(&systemTime);
    return result != 0;
#endif
#ifdef linux
    timeval currentTime;
    if (gettimeofday(&currentTime, nullptr) != 0)
        return false;
    currentTime.tv_sec = dateTime / 1000;
    currentTime.tv_usec = long((dateTime % 1000) * 1000);

    int result = settimeofday(&currentTime, nullptr);
    return 0 == result;
#endif
    return false;
}

void DateTimeUtils::sleepMilliseconds(int milliseconds)
{
#ifdef linux
    timespec ts{0, 0};
    ts.tv_sec  = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, nullptr);
#endif
#ifdef WIN32
    Sleep(milliseconds);
#endif
}

uint64_t DateTimeUtils::getTickCount()
{
#ifdef linux
    timespec ts{0, 0};
    uint64_t tickCountMsecs = -1;
    int result = clock_gettime(CLOCK_MONOTONIC, &ts);
    if (0 == result)
    {
        tickCountMsecs = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }
    return tickCountMsecs;
#endif

#ifdef WIN32
    if (!_getTickCount)
    {
        HINSTANCE hModule = ::LoadLibraryA("kernel32.dll");
        if (hModule)
        {
            _getTickCount = (GetTickCount64Prototype*)::GetProcAddress((HMODULE)hModule, "GetTickCount64");
        }
    }

    if (_getTickCount)
        return (*_getTickCount)();

    return 0;
#endif

    return 0;

}

uint64_t DateTimeUtils::getNextTickCount(uint64_t milliseconds)
{
    auto result = getTickCount();
    if (result > 0)
        result += milliseconds;
    return result;
}

}

#include "DateTimeUtils.h"

#include <chrono>
#include <iomanip>

namespace Threader::Utils {

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

#ifdef WIN32
GetTickCount64Prototype* DateTimeUtils::_getTickCount = nullptr;
#endif

UnuxTimeMSecs DateTimeUtils::msecsSinceEpochLocal()
{
    auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return millisec_since_epoch.count();
}

std::string DateTimeUtils::msecsSinceEpochToStdString(const UnuxTimeMSecs &msecsSinceEpoch,
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

void DateTimeUtils::sleepMilliseconds(int milliseconds)
{
#ifdef linux
    timespec ts{0, 0};
    ts.tv_sec  = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, nullptr);
#endif
#ifdef _WIN32
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

#ifdef _WIN32
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

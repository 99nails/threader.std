#pragma once

#include <stdint.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#endif


namespace Threader {

namespace Utils {

using UnuxTimeMSecs = uint64_t;

#ifdef _WIN32
typedef ULONGLONG (WINAPI GetTickCount64Prototype)(VOID);
#endif

class DateTimeUtils
{
public:
    static UnuxTimeMSecs msecsSinceEpochLocal();

    static std::string msecsSinceEpochToStdString(const UnuxTimeMSecs &msecsSinceEpoch,
                                                  const bool addMsecs,
                                                  const bool isLocal = true,
                                                  const std::string format = "%Y-%m-%d %H:%M:%S");

    static void sleepMilliseconds(int milliseconds);

    static uint64_t getTickCount();

    static uint64_t getNextTickCount(uint64_t milliseconds);

private:
#ifdef _WIN32
    static GetTickCount64Prototype* _getTickCount;
#endif
};


}}

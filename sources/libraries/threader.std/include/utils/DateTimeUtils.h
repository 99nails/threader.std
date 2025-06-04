#pragma once

#include <stdint.h>
#include <string>

namespace Threader {

namespace Utils {

using UnuxTimeMSecs = uint64_t;

class DateTimeUtils
{
public:
    static UnuxTimeMSecs msecsSinceEpochLocal();

    static std::string msecsSinceEpochToStdString(const UnuxTimeMSecs &msecsSinceEpoch,
                                                  const bool addMsecs,
                                                  const bool isLocal = true,
                                                  const std::string format = "%F %T");

    static void sleepMilliseconds(int milliseconds);

    static uint64_t getTickCount();

    static uint64_t getNextTickCount(uint milliseconds);
};


}}

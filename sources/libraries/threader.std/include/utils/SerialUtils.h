#pragma once

#include "ThreadsCommon.h"

#include <string>

namespace Threader::Utils {

class SerialUtils
{
public:
    static Descriptor openSerial(const std::string &deviceName,
                                 const uint &baudRate,
                                 const bool &enableParity,
                                 const bool &twoStopBits,
                                 const uint &dataSize);

    static void closeSerial(Descriptor descriptor);

    static int inputBytesAvailable(Descriptor descriptor);

    static int outputBytesAvailable(Descriptor descriptor);

#ifdef _WIN32
    static bool setWaitMask(Descriptor descriptor, bool needsToWrite);
#endif
};

}

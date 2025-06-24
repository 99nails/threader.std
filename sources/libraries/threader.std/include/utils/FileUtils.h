#pragma once

#include "ThreadsCommon.h"

#include <string>

namespace Threader::Utils {

class FileUtils
{
public:
    static Descriptor fileOpenOrCreate(const std::string &fileName);
    static void fileClose(Descriptor descriptor);
    static bool isPathExists(const std::string &path);
};

}

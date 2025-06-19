#pragma once

#include "ThreadsCommon.h"

#include <string>

namespace Threader {

namespace Utils {

class FileUtils
{
public:
    static Descriptor fileOpenOrCreate(const std::string &fileName);
    static bool isPathExists(const std::string &path);
};

}
} // namespace Threader

#pragma once

#include "ThreadsCommon.h"

#include <string>

namespace Threader {

namespace Utils {

class FileUtils
{
public:
    static Descriptor fileOpenOrCreate(std::string fileName);
};

}
} // namespace Threader

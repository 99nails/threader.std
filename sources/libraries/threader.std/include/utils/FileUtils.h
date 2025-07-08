#pragma once

#include "ThreadsCommon.h"

#include <string>

namespace Threader::Utils {

class FileUtils
{
public:
    static char DELIMITER;

    static Descriptor fileOpenOrCreate(const std::string &fileName);
    static void fileClose(Descriptor descriptor);

    static bool isPathExists(const std::string &path);
    static bool isFileExists(const std::string &fieName);
    static std::string absolutePath(const std::string &relativePath);
    static bool makeDir(const std::string &path);
};

}

#pragma once

#include "ThreadsCommon.h"

#include <string>

namespace Threader::Utils {

class FileUtils
{
public:
    static char DELIMITER;

    static Descriptor fileOpenOrCreate(const std::string &fileName);
    static Descriptor fileCreate(const std::string &fileName);
    static void fileClose(Descriptor descriptor);
    static size_t fileWrite(const Descriptor &descriptor,
                            const std::string &data);

    static bool isPathExists(const std::string &path);
    static bool isFileExists(const std::string &fieName);
    static std::string absolutePath(const std::string &relativePath);
    static bool makeDir(const std::string &path);

    static std::string extractFilePath(const std::string &fileName);
    static std::string extractFileName(const std::string &fileName);
    static std::string extractFileExtension(const std::string &fileName);
};

}

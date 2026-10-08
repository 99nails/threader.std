#pragma once

#include "ThreadsCommon.h"
#include "MemoryBlock.h"

#include <dirent.h>
#include <list>
#include <string>

namespace Threader::Utils {

using FilenameList = std::list<std::string>;

class FileUtils
{
public:
    static Descriptor fileOpenOrCreate(const std::string &fileName);
    static Descriptor fileCreate(const std::string &fileName);
    static Descriptor fileOpen(const std::string &fileName);
    static void fileClose(Descriptor descriptor);
    static void fileSeek(Descriptor descriptor, int mode, int offset);
    static size_t fileRead(const Descriptor descriptor, void *buffer, size_t count);
    static MemoryBlock::Ptr fileReadAll(const Descriptor descriptor);
    static size_t fileWrite(const Descriptor &descriptor,
                            const std::string &data);
    static ssize_t fileSize(const std::string &fileName);
    static ssize_t fileSize(const Descriptor &descriptor);

    static bool isPathExists(const std::string &path);
    static bool isFileExists(const std::string &fileName);
    static bool isRegularFileExists(const std::string &fileName);
    static std::string absolutePath(const std::string &relativePath);

    static bool makeDir(const std::string &path);
    static bool fileDelete(const std::string &fileName);

    static std::string extractFilePath(const std::string &fileName);
    static std::string extractFileName(const std::string &fileName);
    static std::string extractFileExtension(const std::string &fileName);

    static FilenameList scanFiles(const std::string &path);

    static constexpr char DELIMITER =
#ifdef linux
        '/';
#endif
#ifdef WIN32
    '\\';
#endif

};

}

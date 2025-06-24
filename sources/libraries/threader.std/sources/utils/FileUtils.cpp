#include "FileUtils.h"

#ifdef linux
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#include <io.h>
#endif

#include <stdio.h>
#include <sys/stat.h>

namespace Threader::Utils {

Descriptor FileUtils::fileOpenOrCreate(const std::string &fileName)
{
#ifdef linux
    Descriptor descriptor = ::open(fileName.c_str(), O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
#endif
#ifdef _WIN32
    Descriptor descriptor = CreateFileA((LPCSTR)fileName.c_str(),
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        nullptr,
                                        CREATE_ALWAYS,
                                        0, 0);

#endif
    return descriptor;
}

void FileUtils::fileClose(Descriptor descriptor)
{
#ifdef linux
    ::close(descriptor);
#endif
#ifdef _WIN32
    CloseHandle(descriptor);
#endif
}

bool FileUtils::isPathExists(const std::string &path)
{
#ifdef linux
    struct stat sb;
    return stat(path.c_str(), &sb) == 0
            && S_ISDIR(sb.st_mode);
#endif
#ifdef _WIN32
    _finddata_t data;
    long nFind = _findfirst(path.c_str(), &data);
    _findclose(nFind);
    return nFind != -1;
#endif
}

}

#include "FileUtils.h"

#ifdef linux
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifdef WIN32
#include <io.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>

namespace Threader::Utils {

char FileUtils::DELIMITER =
#ifdef linux
        '/';
#endif
#ifdef WIN32
        '\';'
#endif


Descriptor FileUtils::fileOpenOrCreate(const std::string &fileName)
{
#ifdef linux
    Descriptor descriptor = ::open(fileName.c_str(), O_APPEND | O_RDWR, S_IWUSR | S_IRUSR | S_IROTH);
#endif
#ifdef WIN32
    Descriptor descriptor = CreateFileA((LPCSTR)fileName.c_str(),
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        nullptr,
                                        OPEN_ALWAYS,
                                        0, 0);

#endif
    return descriptor;
}

Descriptor FileUtils::fileCreate(const std::string &fileName)
{
#ifdef linux
    Descriptor descriptor = ::open(fileName.c_str(), O_CREAT | O_RDWR, S_IWUSR | S_IRUSR | S_IROTH);
#endif
#ifdef WIN32
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
#ifdef WIN32
    CloseHandle(descriptor);
#endif
}

size_t FileUtils::fileWrite(const Descriptor &descriptor, const std::string &data)
{
#ifdef linux

    return ::write(descriptor, data.c_str(), data.length());

#endif
#ifdef _WIN32
        DWORD writtenCount;
        WriteFile(_descriptor, idata.c_str(), data.length(), &writtenCount, nullptr);
        return writtenCount;
#endif
}

bool FileUtils::isPathExists(const std::string &path)
{
#ifdef linux
    struct stat sb;
    return stat(path.c_str(), &sb) == 0
            && S_ISDIR(sb.st_mode);
#endif
#ifdef WIN32
    _finddata_t data;
    long nFind = _findfirst(path.c_str(), &data);
    _findclose(nFind);
    return nFind != -1;
#endif
}

bool FileUtils::isFileExists(const std::string &fieName)
{
#ifdef linux
    struct stat sb;
    return stat(fieName.c_str(), &sb) == 0
           && S_ISREG(sb.st_mode);
#endif
#ifdef WIN32
    _finddata_t data;
    long nFind = _findfirst(fieName.c_str(), &data);
    _findclose(nFind);
    return nFind != -1 && data.;
#endif
}

std::string FileUtils::absolutePath(const std::string &relativePath)
{
    char result[PATH_MAX] = "";

#ifdef linux
    realpath(relativePath.c_str(), result);
#endif
#ifdef WIN32
    GetFullPathName(relativePath.c_str(),
                    strlen(pathFrom),
                    result,
                    nullptr);
#endif
    return result;
}

bool FileUtils::makeDir(const std::string &path)
{
    return mkdir(path.c_str()
#ifdef linux
                        , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
#endif
                 ) == 0;
}

std::string FileUtils::extractFilePath(const std::string &fileName)
{
    auto slashIndex = fileName.rfind(DELIMITER);
    return (std::string::npos == slashIndex) ? std::string()
                                             : fileName.substr(0, slashIndex + 1);
}

std::string FileUtils::extractFileName(const std::string &fileName)
{
    auto slashIndex = fileName.rfind(DELIMITER);
    return (std::string::npos == slashIndex)
            ? fileName
            : (slashIndex < fileName.length() - 1)
              ? fileName.substr(slashIndex + 1)
              : "";
}

std::string FileUtils::extractFileExtension(const std::string &fileName)
{
    auto shortName = extractFileName(fileName);
    auto pointIndex = shortName.rfind('.');
    return (std::string::npos == pointIndex) ? ""
                                             : fileName.substr(pointIndex);
}

}

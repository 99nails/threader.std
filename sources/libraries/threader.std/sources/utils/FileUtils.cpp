#include "FileUtils.h"

#ifdef linux
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifdef WIN32
#include <io.h>
#include <stdint.h>
#endif

#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>

namespace Threader::Utils {

Descriptor FileUtils::fileOpenOrCreate(const std::string &fileName)
{
#ifdef linux
    Descriptor descriptor = ::open(fileName.c_str(), O_CREAT | O_APPEND | O_RDWR, S_IWUSR | S_IRUSR | S_IROTH);
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
    Descriptor descriptor = ::open(fileName.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR | S_IROTH);
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

Descriptor FileUtils::fileOpen(const std::string &fileName)
{
#ifdef linux
    Descriptor descriptor = ::open(fileName.c_str(), O_RDWR, S_IWUSR | S_IRUSR | S_IROTH);
#endif
#ifdef WIN32
    Descriptor descriptor = CreateFileA((LPCSTR)fileName.c_str(),
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        nullptr,
                                        OPEN_EXISTING,
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

void FileUtils::fileSeek(Descriptor descriptor, int mode, int offset)
{
#ifdef linux
    lseek(descriptor, offset, mode);
#endif
#ifdef WIN32
    SetFilePointer(descriptor, offset, NULL, mode);
#endif
}

size_t FileUtils::fileRead(const Descriptor descriptor, void *buffer, size_t count)
{
#ifdef linux
    ssize_t readCount = 0;
    readCount = ::read(descriptor, buffer, count);
    return readCount;
#endif
#ifdef WIN32
    DWORD readCount = 0;
    ReadFile(descriptor, buffer, count, &readCount, nullptr);
    return readCount;
#endif
}

MemoryBlock::Ptr FileUtils::fileReadAll(const Descriptor descriptor)
{
    if (descriptor == INVALID_DESCRIPTOR)
        return nullptr;

    const uint32_t BUFFER_SIZE = 1024;
    void *buffer = calloc(1, BUFFER_SIZE);
    auto result = std::make_shared<MemoryBlock>(BUFFER_SIZE);

#ifdef linux
    ssize_t readCount = 0;
#endif
#ifdef WIN32
    DWORD readCount = 0;
#endif

    do
    {
#ifdef linux
        readCount = ::read(descriptor, buffer, BUFFER_SIZE);
#endif

#ifdef WIN32
        ReadFile(descriptor, buffer, BUFFER_SIZE, &readCount, nullptr);
#endif
        if (readCount > 0)
            result->append(buffer, readCount);
    }
    while (readCount == BUFFER_SIZE);

    free(buffer);

    return result;
}

size_t FileUtils::fileWrite(const Descriptor &descriptor, const std::string &data)
{
#ifdef linux

    return ::write(descriptor, data.c_str(), data.length());

#endif
#ifdef WIN32
    DWORD writtenCount;
    WriteFile(descriptor, data.c_str(), data.length(), &writtenCount, nullptr);
    return writtenCount;
#endif
}

ssize_t FileUtils::fileSize(const std::string &fileName)
{
    struct stat fileInfo;

    if (stat(fileName.c_str(), &fileInfo) == 0)
        return fileInfo.st_size;

    return -1;
}

ssize_t FileUtils::fileSize(const Descriptor &descriptor)
{
#ifdef linux
//    auto result = lseek(descriptor, 0, SEEK_END);
    struct stat fileInfo;
    fstat(descriptor, &fileInfo);
    return fileInfo.st_size;
#endif
#ifdef WIN32
    return SetFilePointer(descriptor, 0, nullptr, SEEK_END);
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
    DWORD attrs = GetFileAttributesA(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
#endif
}

bool FileUtils::isFileExists(const std::string &fileName)
{
#ifdef linux
    struct stat sb;
    return stat(fileName.c_str(), &sb) == 0;
#endif
#ifdef WIN32
    DWORD attrs = GetFileAttributesA(fileName.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
#endif
}

bool FileUtils::isRegularFileExists(const std::string &fileName)
{
#ifdef linux
    struct stat sb;
    return stat(fileName.c_str(), &sb) == 0
            && S_ISREG(sb.st_mode);
#endif
#ifdef WIN32
    DWORD attrs = GetFileAttributesA(fileName.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
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
                    PATH_MAX,
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

bool FileUtils::fileDelete(const std::string &fileName)
{
    return 0 == std::remove(fileName.c_str());
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
                                             : shortName.substr(pointIndex);
}

#ifdef linux

int filter_file(const struct dirent *entry) {
    return (entry->d_type != DT_DIR);
}

FilenameList FileUtils::scanFiles(const std::string &path)
{
    struct dirent **namelist;
    int n = scandir(path.c_str(), &namelist, filter_file, alphasort);
    FilenameList result;

    if (n >= 0)
    {
        for (int i = 0; i < n; i++)
        {
            result.push_back(namelist[i]->d_name);
            free(namelist[i]);
        }
        free(namelist);
    }

    return result;
}

#endif

#ifdef WIN32

FilenameList FileUtils::scanFiles(const std::string &path)
{
    WIN32_FIND_DATA fd;

    Descriptor hFind = FindFirstFile(path.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return {};
    }

    FilenameList files;
    do
    {
        const std::string fileName(fd.cFileName);
        // Пропускаем . и ..
        if (fileName == "." || fileName == "..")
            continue;

        // Только обычные файлы (не каталоги)
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // Формируем полный путь, если нужно:
            // L"C:\\path\\to\\folder\\" + fd.cFileName
            files.emplace_back(fd.cFileName);
        }
    }
    while (FindNextFile(hFind, &fd));

    FindClose(hFind);

    return files;
}

#endif

}

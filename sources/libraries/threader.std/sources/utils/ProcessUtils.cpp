#include "ProcessUtils.h"

#include "FileUtils.h"

#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

namespace Threader::Utils {


#ifdef linux
bool ProcessUtils::readProcessInfo(pid_t pid, ProcessInfo &processInfo)
{
    const std::string cmdLineFileName = "/proc/" + std::to_string(pid) + "/cmdline";
    if (!FileUtils::isFileExists(cmdLineFileName))
        return false;

    const std::string shortCommandFileName = "/proc/" + std::to_string(pid) + "/comm";
    if (!FileUtils::isFileExists(shortCommandFileName))
        return false;

    auto descriptor = open(cmdLineFileName.c_str(), O_RDONLY);
    if (descriptor == INVALID_DESCRIPTOR)
        return false;;

    bool result = true;

    auto data = FileUtils::fileReadAll(descriptor);
    close(descriptor);

    if (data->size() > 0)
    {
        size_t length = 0;
        size_t start = 0;

        processInfo = {.pid = pid,
                       .processName = std::string(),
                       .binaryWithPath = std::string(),
                       .commandLine = std::string(),
                       .arguments = {}};

        char* buffer = static_cast<char*>(data->data());
        while (length < data->size())
        {
            if (buffer[length] == '\0')
            {
                std::string word(buffer + start, buffer + length);

                if (processInfo.processName.empty())
                {
                    processInfo.processName = FileUtils::extractFileName(word);
                    processInfo.binaryWithPath = word;
                }
                else
                {
                    processInfo.arguments.push_back(word);
                }

                start = length + 1;

                if (length + 1 < static_cast<size_t>(data->size()))
                    buffer[length] = ' ';
            }
            ++length;
        }
        processInfo.commandLine = buffer;
    }
    else
        result = false;

    descriptor = open(shortCommandFileName.c_str(), O_RDONLY);
    if (descriptor != INVALID_DESCRIPTOR)
    {
        data = FileUtils::fileReadAll(descriptor);
        close(descriptor);
        processInfo.shortCommand = std::string((char*)data->data(), data->size());
    }

    return result;
}

ProcessInfoList ProcessUtils::enumerate(ProcessEnumerationCallback &callback)
{

    DIR* dir = opendir("/proc");
    if (!dir)
    {
        perror("opendir /proc");
        return {};
    }

    ProcessInfoList result;
    struct dirent* entry;

    while ((entry = readdir(dir)) != nullptr)
    {
        // Пропускаем всё, что не похоже на PID (цифры)
        std::string directoryName = entry->d_name;
        if (directoryName.find_first_not_of("0123456789") != std::string::npos)
            continue;

        pid_t pid = std::stoul(directoryName);
        ProcessInfo processInfo;
        if (readProcessInfo(pid, processInfo))
        {
            if (callback(processInfo))
                result.push_back(processInfo);
        }
    }

    return result;
}

#endif

}

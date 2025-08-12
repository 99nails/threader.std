
#ifdef linux

#include <cxxabi.h>
#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <iostream>

#include "DateTimeUtils.h"
#include "FileUtils.h"
#include "StdStringUtils.h"
#include "ThreadLog.h"

#ifndef STACK_LEVEL
#define STACK_LEVEL 50
#endif

pid_t gettid()
{
    return pid_t(syscall( __NR_gettid ));
}

void handleDeathSignal(int signum);

static std::string threadInfo;

void setThreadInfo(const std::string & info)
{
    threadInfo = info;
}

std::string get_signal_string(int signum);

#define make_signal_name_num(SIG) {#SIG, SIG}

#pragma pack(push, 4)
struct
{
    const char *signame;
    int signum;
}

signal_name_num_list[] =
{
    make_signal_name_num(SIGSEGV),
    make_signal_name_num(SIGABRT),
    make_signal_name_num(SIGILL),
    make_signal_name_num(SIGKILL),
    make_signal_name_num(SIGSTOP),
    make_signal_name_num(SIGBUS),
    make_signal_name_num(SIGSYS),
    make_signal_name_num(SIGFPE)
};
#pragma pack(pop)

static std::string applicationPath;
static std::string applicationBinaryFileName;

void setSignalHandler(const char* arg0)
{
    std::string binaryFilePath(arg0);
    auto slashIndex = binaryFilePath.rfind("/");
    applicationBinaryFileName = (slashIndex == std::string::npos)
            ? binaryFilePath
            : (slashIndex < binaryFilePath.length() - 1)
              ? binaryFilePath.substr(slashIndex + 1)
              : "LogFile";

    applicationPath = (slashIndex == 0) ? ""
                                        : binaryFilePath.substr(0, slashIndex + 1);

    for (auto & signal_name_num : signal_name_num_list)
        signal(signal_name_num.signum, handleDeathSignal);
}

using namespace Threader::Threads;

std::string demangle(const char* name)
{
    int status;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result = std::string((0 == status) ? demangled : name);

    if (demangled)
        free(demangled);

    return result;
}

std::string demangleFunctionName(char* line)
{
    std::string result{line};

    auto start = result.find('(');
    auto end = result.find('+');
    if (start > 0 && end > 0 && start < end)
    {
        std::string name = result.substr(start + 1, end - start - 1);
        if (!name.empty())
        {
            name = demangle(name.c_str());
            result.replace(start + 1, end - start - 1, name);
        }
    }

    return result;
}

void handleDeathSignal(int signum)
{
    if(signum == SIGABRT)
    {
        signal(signum, nullptr);
        exit(3);
    }

    void *buffer[STACK_LEVEL];
    char **strings;
    int num = backtrace(buffer, STACK_LEVEL);
    strings = backtrace_symbols(buffer, num);

    std::string errorFileName = applicationPath + "Errors";
    auto status = mkdir(errorFileName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if (0 == status || EEXIST == errno)
    {
        auto dateValue = DateTimeUtils::msecsSinceEpochLocal();
        auto dateString = DateTimeUtils::msecsSinceEpochToStdString(dateValue, false, true, "%Y-%m-%d");
        auto dateTimeString = DateTimeUtils::msecsSinceEpochToStdString(dateValue, true);
        errorFileName += std::string(format("/%s-%s.Error", applicationBinaryFileName.c_str(), dateString.c_str()));

        auto file = FileUtils::fileOpenOrCreate(errorFileName);
        lseek(file, 0, SEEK_END);

        if (file != INVALID_DESCRIPTOR)
        {
            std::string content;

            content += "\r\n\r\n" + dateTimeString + "\r\n";
            content += get_signal_string(signum) + "\r\n";
            content += "ThreadId:" + std::to_string(syscall(SYS_gettid)) + "\r\n";
            content += "Thread info: " + threadInfo;
            if (strings)
            {
                for (int j = 0; j < num; j++)
                    content += demangleFunctionName(strings[j]) + "\r\n";
                free(strings);
            }
            write(file, content.c_str(), content.length());

            close(file);
        }
    }

    if (ThreadBase::logThread())
    {
        auto logThread = dynamic_cast<ThreadLog*>(ThreadBase::logThread());
        if (logThread)
            logThread->flush();
    }

    signal(signum, nullptr);
    kill(gettid(), signum);
    exit(3);
}

std::string get_signal_string(int signum)
{
    std::string tmp("Signal: ");
    for (auto & i : signal_name_num_list)
    {
        if (signum == i.signum)
        {
            tmp += i.signame;
            return tmp;
        }
    }
    tmp +=  std::to_string(signum);
    return tmp;
}

#endif

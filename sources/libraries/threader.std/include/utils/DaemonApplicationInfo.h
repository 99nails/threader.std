#pragma once

#include "CommandLineParser.h"

#include <vector>
#include <string>

using namespace Threader::Threads;

namespace Threader::Utils
{


class DaemonApplicationInfo
{
private:
    explicit DaemonApplicationInfo();

    DaemonApplicationInfo(DaemonApplicationInfo const&) = delete;

    void operator=(DaemonApplicationInfo const&) = delete;

public:
    static DaemonApplicationInfo& instance()
    {
        static DaemonApplicationInfo instance;
        return instance;
    }

    void setArguments(int argc, char* argv[]);

    const CommandLineParser &commandLine();

    const std::string &binaryFileName() const;

    const std::string &binaryPath() const;

private:
    std::vector<std::string> _arguments;
    CommandLineParser _commandLine;

    std::string _binaryFileName;
    std::string _binaryPath;
};

#define appInfo() DaemonApplicationInfo::instance()

}

#include "DaemonApplicationInfo.h"

#include "FileUtils.h"

namespace Threader::Utils
{

DaemonApplicationInfo::DaemonApplicationInfo()
    : _arguments({})
    , _binaryFileName(std::string())
    , _binaryPath(std::string())
{
}

void DaemonApplicationInfo::setArguments(int argc, char *argv[])
{
    _arguments.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        _arguments.push_back(argv[i]);
    }
    _commandLine.setArguments(_arguments);

    _binaryFileName = FileUtils::extractFileName(_commandLine.token(0));
    _binaryPath = FileUtils::extractFilePath(FileUtils::absolutePath(_commandLine.token(0)));
}

const CommandLineParser &DaemonApplicationInfo::commandLine()
{
    return _commandLine;
}

const std::string &DaemonApplicationInfo::binaryFileName() const
{
    return _binaryFileName;
}

const std::string &DaemonApplicationInfo::binaryPath() const
{
    return _binaryPath;
}

}

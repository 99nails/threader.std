#include "DaemonApplicationInfo.h"

#include "FileUtils.h"

namespace Threader::Utils
{

DaemonApplicationInfo::DaemonApplicationInfo()
{
}

void DaemonApplicationInfo::setArguments(int argc, char *argv[])
{
    _arguments.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        _arguments.push_back(argv[i]);
    }
    _commandLine = CommandLineParser(_arguments);

    _binaryFileName = FileUtils::extractFileName(appInfo().commandLine().token(0));
    _binaryPath = FileUtils::extractFilePath(FileUtils::absolutePath(appInfo().commandLine().token(0)));
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

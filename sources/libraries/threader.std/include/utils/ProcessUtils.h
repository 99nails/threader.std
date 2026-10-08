#pragma once

#include <functional>
#include <list>
#include <string>

namespace Threader::Utils {

typedef struct
{
    pid_t pid;
    std::string processName;
    std::string binaryWithPath;
    std::string commandLine;
    std::string shortCommand;
    std::list<std::string> arguments;

} ProcessInfo;

using ProcessInfoList = std::list<ProcessInfo>;

using ProcessEnumerationCallback = std::function<bool(const ProcessInfo &)>;

class ProcessUtils
{
public:
#ifdef linux
    static bool readProcessInfo(pid_t pid, ProcessInfo &processInfo);

    static ProcessInfoList enumerate(ProcessEnumerationCallback &callback);
#endif
};

}

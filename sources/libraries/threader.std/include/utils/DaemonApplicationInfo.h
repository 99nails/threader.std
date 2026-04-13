#pragma once

#include <vector>
#include <string>

namespace Threader::Utils
{

class DaemonApplicationInfo
{
private:
    explicit DaemonApplicationInfo();
    DaemonApplicationInfo(DaemonApplicationInfo const&) = delete;
    void operator=(DaemonApplicationInfo const&) = delete;

public:
    static DaemonApplicationInfo& instance() {
        static DaemonApplicationInfo instance;
        return instance;
    }

    void setArguments(int argc, char* argv[]);

private:
    std::vector<std::string> _arguments;
};

}

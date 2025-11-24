#pragma once

#include <string>

namespace Threader::Utils {

class DaemonUtils
{
public:
    static bool registerDaemon(const std::string &serviceName,
                               const std::string &binaryFileName,
                               const std::string &description);
    static bool unregisterDaemon(const std::string &serviceName);
};

}

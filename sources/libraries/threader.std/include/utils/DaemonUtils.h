#pragma once

#include <semaphore.h>
#include <string>

namespace Threader::Utils {

class DaemonUtils
{
public:
    static bool daemonRegister(const std::string &serviceName,
                               const std::string &binaryFileName,
                               const std::string &description);

    static bool daemonUnregister(const std::string &serviceName);

    static bool daemonSetRunning(const std::string &serviceName, const bool isRunning);

    static bool daemonIsRunning(const std::string &serviceName);

private:
    static sem_t *_semaphore;
};

}

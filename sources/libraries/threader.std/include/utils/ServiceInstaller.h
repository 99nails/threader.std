#pragma once

#include "ServiceBase.h"

namespace Threader::Utils
{

class ServiceInstaller {
public:
    static bool Install(const ServiceBase& service,
                        std::string &resultString);

    static bool Uninstall(const ServiceBase& service,
                          std::string &resultString);
private:
    ServiceInstaller() {}
};

}

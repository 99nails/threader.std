#ifndef SERVICEINSTALLER_H
#define SERVICEINSTALLER_H

#include "ServiceBase.h"

class ServiceInstaller {
public:
    static bool Install(const ServiceBase& service);
    static bool Uninstall(const ServiceBase& service);
private:
    ServiceInstaller() {}
};


#endif // SERVICEINSTALLER_H

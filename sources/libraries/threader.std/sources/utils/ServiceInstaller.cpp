#include "ServiceInstaller.h"

#include <iostream>

namespace
{

class ServiceHandle
{
public:
    ServiceHandle(const SC_HANDLE &handle)
        : _handle(handle)
    {
    }

    ~ServiceHandle()
    {
        if (_handle)
        {
            ::CloseServiceHandle(_handle);
        }
    }

    operator SC_HANDLE()
    {
        return _handle;
    }

private:
    SC_HANDLE _handle;
};

}

bool ServiceInstaller::Install(const ServiceBase &service)
{
    TCHAR modulePath[MAX_PATH];

    if (::GetModuleFileName(nullptr, modulePath, MAX_PATH) == 0)
    {
        std::cout << "Couldn't get module file name: " << std::endl;
        return false;
    }

    ServiceHandle serviceControlManager = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (!serviceControlManager)
    {
        std::cout << "Couldn't open service control manager: " << errorString() << std::endl;
        return false;
    }

    const std::string& depends = service.GetDependencies();
    const std::string& account = service.GetAccount();
    const std::string& password = service.GetPassword();

    ServiceHandle serviceHandle = ::CreateService(serviceControlManager,
                                                  service.GetName().c_str(),
                                                  service.GetDisplayName().c_str(),
                                                  SERVICE_QUERY_STATUS,
                                                  SERVICE_WIN32_OWN_PROCESS,
                                                  service.GetStartType(),
                                                  service.GetErrorControlType(),
                                                  modulePath,
                                                  nullptr,
                                                  nullptr,
                                                  (depends.empty() ? nullptr : depends.c_str()),
                                                  (account.empty() ? nullptr : account.c_str()),
                                                  (password.empty() ? nullptr : password.c_str()));

    if (!serviceHandle)
    {
        std::cout << "Couldn't create service: " << errorString() << std::endl;
        return false;
    }

    std::cout << "Service installed sucesessfully" << std::endl;
    return true;
}

bool ServiceInstaller::Uninstall(const ServiceBase& service)
{
    ServiceHandle svcControlManager = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);

    if (!svcControlManager)
    {
        std::cout << "Couldn't open service control manager: " << errorString() << std::endl;
        return false;
    }

    ServiceHandle serviceHandle = ::OpenService(svcControlManager, service.GetName().c_str(),
                                                SERVICE_QUERY_STATUS | SERVICE_STOP | DELETE);

    if (!serviceHandle)
    {
        std::cout << "Couldn't open service: " << errorString() << std::endl;
        return false;
    }

    SERVICE_STATUS serviceStatus = {};

    if (!::QueryServiceStatus(serviceHandle, &serviceStatus))
    {
        std::cout << "Failed to query the service : " << errorString();
        return false;
    }

    if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
    {
        if (::ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus))
        {
            std::cout << "Stoping service " << service.GetName() << std::endl;

            while (::QueryServiceStatus(serviceHandle, &serviceStatus))
            {
                if (serviceStatus.dwCurrentState != SERVICE_STOP_PENDING)
                {
                    break;
                }
            }

            if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
            {
                std::cout << "Failed to stop the service " << errorString();
            }
            else
            {
                std::cout << "Service stopped\n";
            }
        }
        else
        {
            std::cout << "Didn't control service: " << errorString() << std::endl;
        }
    }

    if (!::DeleteService(serviceHandle))
    {
        std::cout << "Failed to delete the service: " << errorString() << std::endl;
        return false;
    }

    std::cout << "Service uninstalled sucesessfully" << std::endl;
    return true;
}



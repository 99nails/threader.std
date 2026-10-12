#include "ServiceInstaller.h"

#include "StdStringUtils.h"

#include <iostream>

namespace Threader::Utils
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


bool ServiceInstaller::Install(const ServiceBase &service,
                               std::string &resultString)
{
    TCHAR modulePath[MAX_PATH];

    if (::GetModuleFileName(nullptr, modulePath, MAX_PATH) == 0)
    {
        resultString = "Couldn't get module file name: " + errorString();
        return false;
    }

    ServiceHandle serviceControlManager = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (!serviceControlManager)
    {
        resultString = "Couldn't open service control manager: " + errorString();
        return false;
    }

    const auto displayName = StdStringUtils::ConvertUTF8ToCP1251(service.displayName());
    const auto &depends = service.dependencies();
    const auto &account = service.account();
    const auto &password = service.password();

    ServiceHandle serviceHandle = ::CreateService(serviceControlManager,
                                                  service.name().c_str(),
                                                  displayName.c_str(),
                                                  SERVICE_QUERY_STATUS,
                                                  SERVICE_WIN32_OWN_PROCESS,
                                                  service.startType(),
                                                  service.errorControlType(),
                                                  modulePath,
                                                  nullptr,
                                                  nullptr,
                                                  (depends.empty() ? nullptr : depends.c_str()),
                                                  (account.empty() ? nullptr : account.c_str()),
                                                  (password.empty() ? nullptr : password.c_str()));

    if (!serviceHandle)
    {
        resultString = "Couldn't create service: " + errorString();
        return false;
    }

    resultString = "Service installed sucesessfully";
    return true;
}

bool ServiceInstaller::Uninstall(const ServiceBase& service,
                                 std::string &resultString)
{
    ServiceHandle svcControlManager = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);

    if (!svcControlManager)
    {
       resultString = "Couldn't open service control manager: " + errorString();
        return false;
    }

    ServiceHandle serviceHandle = ::OpenService(svcControlManager, service.name().c_str(),
                                                SERVICE_QUERY_STATUS | SERVICE_STOP | DELETE);

    if (!serviceHandle)
    {
       resultString = "Couldn't open service: " + errorString();
        return false;
    }

    SERVICE_STATUS serviceStatus = {};

    if (!::QueryServiceStatus(serviceHandle, &serviceStatus))
    {
        resultString = "Failed to query the service : " + errorString();
        return false;
    }

    if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
    {
        if (::ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus))
        {
            while (::QueryServiceStatus(serviceHandle, &serviceStatus))
            {
                if (serviceStatus.dwCurrentState != SERVICE_STOP_PENDING)
                {
                    break;
                }
            }

            if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
            {
                resultString = "Failed to stop the service " + errorString();
            }
        }
        else
        {
            resultString = "Didn't control service: " + errorString();
        }
    }

    if (!::DeleteService(serviceHandle))
    {
        resultString = "Failed to delete the service: " + errorString();
        return false;
    }

    resultString = "Service uninstalled sucesessfully";
    return true;
}

}

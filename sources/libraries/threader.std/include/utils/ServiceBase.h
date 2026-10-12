#pragma once
#include <windows.h>
#include <string>

#include "CopyDisabler.h"
#include "ThreadBase.h"

namespace Threader::Utils
{


using namespace Threader::Threads;


std::string errorString(int errorNo = 0);

class ServiceBase
{
public:
    virtual ~ServiceBase() {}

    bool Run();

    const std::string& name() const;

    const std::string& displayName() const;

    DWORD startType() const;

    DWORD errorControlType() const;

    const std::string& dependencies() const;

    const std::string& account() const;

    const std::string& password() const;

protected:
    ServiceBase(const std::string& name,
                const std::string& displayName,
                DWORD dwStartType,
                DWORD dwErrCtrlType = SERVICE_ERROR_NORMAL,
                DWORD dwAcceptedCmds = SERVICE_ACCEPT_STOP,
                const std::string& depends = std::string(),
                const std::string& account = std::string(),
                const std::string& password = std::string());

    void SetStatus(DWORD dwState, DWORD dwErrCode = NO_ERROR, DWORD dwWait = 0);

    static void WriteToEventLog(const std::string &name,
                                const std::string &message,
                                WORD type = EVENTLOG_INFORMATION_TYPE);

    virtual void OnStart(DWORD argc, TCHAR* argv[]) = 0;
    virtual void OnStop() {}
    virtual void OnPause() {}
    virtual void OnContinue() {}
    virtual void OnShutdown() {}

    virtual void OnSessionChange(DWORD /*evtType*/,
                                 WTSSESSION_NOTIFICATION* /*notification*/) {}
private:
    static void WINAPI SvcMain(DWORD argc,
                               TCHAR* argv[]);

    static DWORD WINAPI ServiceCtrlHandler(DWORD ctrlCode,
                                           DWORD evtType,
                                           void* evtData,
                                           void* context);

    static bool RunInternal(ServiceBase* service);

    void Start(DWORD argc, TCHAR* argv[]);
    void Stop();
    void Pause();
    void Continue();
    void Shutdown();

    std::string _name;
    std::string _displayName;
    DWORD _dwStartType;
    DWORD _dwErrorCtrlType;
    std::string _dependencies;
    std::string _account;
    std::string _password;

    // Info about service dependencies and user account.
    bool _hasDependencies;/* = false*/;
    bool _hasAccount;/* = false*/;
    bool _hasPassword;/* = false*/;
    SERVICE_STATUS _serviceStatus;
    SERVICE_STATUS_HANDLE _serviceStatusHandle;

    static ServiceBase* _service;
    DISABLE_COPY_AND_MOVE(ServiceBase)
};

class ServiceThreader : public ServiceBase
{
public:
    ServiceThreader(ThreadBase *thread,
                    const std::string& name,
                    const std::string& displayName,
                    DWORD dwStartType,
                    DWORD dwErrCtrlType = SERVICE_ERROR_NORMAL,
                    DWORD dwAcceptedCmds = SERVICE_ACCEPT_STOP,
                    const std::string& depends = std::string(),
                    const std::string& account = std::string(),
                    const std::string& password = std::string());
protected:
    void OnStart(DWORD argc, TCHAR* argv[]) override;
    void OnStop() override;

private:
    ThreadBase *_thread;
};

}

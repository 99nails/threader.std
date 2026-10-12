//#include <stdafx.h>
#include "ServiceBase.h"
#include <cassert>

#include "Log.h"
#include "Logger.h"


namespace Threader::Utils
{

MESSAGE_TEMPLATE(9999, 0, MessageType::Debug,
                 "Debug: %s");

std::string errorString(int errorNo)
{
    if (0 == errorNo)
        errorNo = GetLastError();

    LPSTR messageBuffer = nullptr;
    FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorNo, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    return std::to_string(errorNo) + " - " + std::string(messageBuffer);
}


/***************************************************************************************************
 * ServiceBase class
 **************************************************************************************************/

ServiceBase* ServiceBase::_service = nullptr;


bool ServiceBase::Run()
{
    return RunInternal(this);
}

const std::string &ServiceBase::name() const
{
    return _name;
}

const std::string &ServiceBase::displayName() const
{
    return _displayName;
}

DWORD ServiceBase::startType() const
{
    return _dwStartType;
}

DWORD ServiceBase::errorControlType() const
{
    return _dwErrorCtrlType;
}

const std::string &ServiceBase::dependencies() const
{
    return _dependencies;
}

const std::string &ServiceBase::account() const
{
    return _account;
}

const std::string &ServiceBase::password() const
{
    return _password;
}

ServiceBase::ServiceBase(const std::string &name,
                         const std::string &displayName,
                         DWORD dwStartType,
                         DWORD dwErrCtrlType,
                         DWORD dwAcceptedCmds,
                         const std::string &depends,
                         const std::string &account,
                         const std::string &password)
    : _name(name),
      _displayName(displayName),
      _dwStartType(dwStartType),
      _dwErrorCtrlType(dwErrCtrlType),
      _dependencies(depends),
      _account(account),
      _password(password),
      _serviceStatusHandle(nullptr)
{

    _hasDependencies = !_dependencies.empty();
    _hasAccount = !_account.empty();
    _hasPassword = !_password.empty();
    _serviceStatus.dwControlsAccepted = dwAcceptedCmds;
    _serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    _serviceStatus.dwWin32ExitCode = NO_ERROR;
    _serviceStatus.dwServiceSpecificExitCode = 0;
    _serviceStatus.dwCheckPoint = 0;
    _serviceStatus.dwWaitHint = 0;
}

void ServiceBase::SetStatus(DWORD dwState,
                            DWORD dwErrCode,
                            DWORD dwWait)
{
    _serviceStatus.dwCurrentState = dwState;
    _serviceStatus.dwWin32ExitCode = dwErrCode;
    _serviceStatus.dwWaitHint = dwWait;
    ::SetServiceStatus(_serviceStatusHandle, &_serviceStatus);
}

void ServiceBase::WriteToEventLog(const std::string &name,
                                  const std::string &message,
                                  WORD type)
{
    HANDLE hSource = RegisterEventSource(nullptr, name.c_str());
    if (hSource)
    {
        const TCHAR* msgData[2] = {name.c_str(), message.c_str()};
        ReportEvent(hSource, type, 0, 0, nullptr, 2, 0, msgData, nullptr);
        DeregisterEventSource(hSource);
    }
}

// static
void WINAPI ServiceBase::SvcMain(DWORD argc,
                                 TCHAR* argv[])
{
    // writeLog(Message9999, "Starting service...");


    assert(_service);
    _service->_serviceStatusHandle = ::RegisterServiceCtrlHandlerEx(_service->name().c_str(),
                                                                    ServiceCtrlHandler,
                                                                    NULL);

    if (!_service->_serviceStatusHandle)
    {
        ServiceBase::WriteToEventLog(_service->_name,
                                     "Can't set service control handler",
                                     EVENTLOG_ERROR_TYPE);
        return;
    }

    _service->Start(argc, argv);
}

DWORD WINAPI ServiceBase::ServiceCtrlHandler(DWORD ctrlCode,
                                             DWORD evtType,
                                             void* evtData,
                                             void* /*context*/)
{
    switch (ctrlCode)
    {
    case SERVICE_CONTROL_STOP:
        _service->Stop();
        break;

    case SERVICE_CONTROL_PAUSE:
        _service->Pause();
        break;

    case SERVICE_CONTROL_CONTINUE:
        _service->Continue();
        break;

    case SERVICE_CONTROL_SHUTDOWN:
        _service->Shutdown();
        break;

    case SERVICE_CONTROL_SESSIONCHANGE:
        _service->OnSessionChange(evtType, reinterpret_cast<WTSSESSION_NOTIFICATION*>(evtData));
        break;

    default:
        break;
    }

    return 0;
}

bool ServiceBase::RunInternal(ServiceBase* service)
{
    _service = service;

    TCHAR* svcName = new TCHAR[_service->name().size() + 1];
    std::copy(_service->name().begin(), _service->name().end(), svcName);

    SERVICE_TABLE_ENTRY tableEntry[] =
    {
        {svcName, SvcMain},
        {nullptr, nullptr}
    };

    return ::StartServiceCtrlDispatcher(tableEntry) == TRUE;
}

void ServiceBase::Start(DWORD argc, TCHAR* argv[])
{
    SetStatus(SERVICE_START_PENDING);
    OnStart(argc, argv);
    SetStatus(SERVICE_RUNNING);
}

void ServiceBase::Stop()
{
    SetStatus(SERVICE_STOP_PENDING);
    OnStop();
    SetStatus(SERVICE_STOPPED);
}

void ServiceBase::Pause()
{
    SetStatus(SERVICE_PAUSE_PENDING);
    OnPause();
    SetStatus(SERVICE_PAUSED);
}

void ServiceBase::Continue()
{
    SetStatus(SERVICE_CONTINUE_PENDING);
    OnContinue();
    SetStatus(SERVICE_RUNNING);
}

void ServiceBase::Shutdown()
{
    OnShutdown();
    SetStatus(SERVICE_STOPPED);
}

/***************************************************************************************************
 * ServiceThreader class
 **************************************************************************************************/

ServiceThreader::ServiceThreader(ThreadBase *thread,
                                 const std::string &name,
                                 const std::string &displayName,
                                 DWORD dwStartType,
                                 DWORD dwErrCtrlType,
                                 DWORD dwAcceptedCmds,
                                 const std::string &depends,
                                 const std::string &account,
                                 const std::string &password)
    : ServiceBase(name,
                  displayName,
                  dwStartType,
                  dwErrCtrlType,
                  dwAcceptedCmds,
                  depends,
                  account,
                  password)
    , _thread(thread)
{
}

void ServiceThreader::OnStart(DWORD argc, TCHAR *argv[])
{
    writeLog(Message9999, "Starting service...");

    if (!_thread)
    {
        writeLog(Message9999, "Thread is NULL");
        return;
    }


    _thread->startThread();
}

void ServiceThreader::OnStop()
{
    if (!_thread)
        return;

    while (!_thread->isThreadFinished())
    {
        _thread->postTerminateEvent();
        DateTimeUtils::sleepMilliseconds(100);
    }
}

}

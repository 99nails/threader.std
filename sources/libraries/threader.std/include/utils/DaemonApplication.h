#pragma once

#include "CommandLineParser.h"
#include "DaemonApplicationInfo.h"
#include "DaemonUtils.h"
#include "DateTimeUtils.h"
#include "LogMessagesTemplates.h"
#include "SignalHandler.h"

#include "Log.h"

#include <csignal>
#include <iostream>

#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Threader::Threads {

using namespace Threader::Utils;

void readLine()
{
    std::string line;
    std::getline(std::cin, line);
}

template<class ThreadMainType>
class DaemonApplication
{
public:
    explicit DaemonApplication(int argc,
                               char *argv[],
                               const std::string &description = std::string(),
                               const std::string &serviceName = std::string());
    virtual int run();
protected:
    bool initialize();
    virtual void runMainLoop();
    virtual void exitMainLoop();
    ThreadMainType *threadMain();

    bool console();
    int logLevel();

private:
    bool parseArguments();
    static void signalHandler(int signal);

private:
    static DaemonApplication *_instance;
    ThreadMainType *_threadMain;
    bool _isRunning = true;
    int _argc;
    std::string _description;
    static std::string _serviceName;

    Descriptor _pipeDescriptors[2];

    bool _console;
    bool _install;
    bool _uninstall;
    int _logLevel;
};

template<class ThreadMainType>
std::string DaemonApplication<ThreadMainType>::_serviceName = std::string();

template<class ThreadMainType>
DaemonApplication<ThreadMainType> *DaemonApplication<ThreadMainType>::_instance = nullptr;

template<class ThreadMainType>
DaemonApplication<ThreadMainType>::DaemonApplication(int argc,
                                                     char *argv[],
                                                     const std::string &description,
                                                     const std::string &serviceName)
    : _threadMain(nullptr)
    , _argc(argc)
    , _description(description)
    , _console(false)
    , _install(false)
    , _uninstall(false)
    , _logLevel(9)
{
    _serviceName = serviceName;
    _instance = this;
    _console = false;

    _pipeDescriptors[0] = INVALID_DESCRIPTOR;
    _pipeDescriptors[1] = INVALID_DESCRIPTOR;

    appInfo().setArguments(argc, argv);
}

template<class ThreadMainType>
int DaemonApplication<ThreadMainType>::run()
{
    if (!initialize())
        return -1;

    if (_install)
    {
        writeLog(Message4, _serviceName.c_str());

        std::string resultString;
        auto registered = DaemonUtils::daemonRegister(_serviceName,
                                                      appInfo().commandLine().token(0),
                                                      _description,
                                                      &resultString);
        if (registered)
        {
            writeLog(Message5, _serviceName.c_str());
            stopLogSubsystem();
            return 0;
        }

        writeLog(Message6, _serviceName.c_str(), resultString.c_str());
        stopLogSubsystem();
        return -1;
    }

    if (_uninstall)
    {
        writeLog(Message7, _serviceName.c_str());

        std::string resultString;
        auto unregistered = DaemonUtils::daemonUnregister(_serviceName,
                                                          &resultString);
        if (unregistered)
        {
            writeLog(Message8, _serviceName.c_str());
            stopLogSubsystem();
            return 0;
        }

        writeLog(Message9, _serviceName.c_str(), resultString.c_str());
        stopLogSubsystem();
        return -1;
    }


    if (DaemonUtils::daemonIsRunning(_serviceName))
    {
        writeLog(Message17, _serviceName.c_str());
    }

    DaemonUtils::daemonSetRunning(_serviceName, true);

    _threadMain = new ThreadMainType(_serviceName, _description);
    _threadMain->startThread();

    runMainLoop();

    _threadMain->postTerminateEvent();
    while (!_threadMain->isThreadFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    DaemonUtils::daemonSetRunning(_serviceName, false);

    stopLogSubsystem();

    return 0;
}

template<class ThreadMainType>
bool DaemonApplication<ThreadMainType>::initialize()
{
#ifdef linux
    setErrorSignalHandler(appInfo().commandLine().token(0).c_str());

    setlocale(LC_ALL, "ru_RU.UTF-8");
    setlocale(LC_NUMERIC, "C");

    std::string line = "Нажмите Enter или Ctrl+C для завершения программы...\r\n";
#endif

#ifdef WIN
    constexpr char MESSAGE_START_HINT[] = "Нажмите Enter для завершения программы...\r\n";
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251(MESSAGE_START_HINT);
#endif
    if (appInfo().commandLine().tokenCount() < 1)
        return false;

    if (!parseArguments())
        return false;

    if (_console)
        std::cout << line;

    startLogSubsystem(appInfo().commandLine().token(0).c_str(), nullptr, _console, _logLevel);

    if (_serviceName.empty())
        _serviceName = appInfo().binaryFileName();

    chdir(appInfo().binaryPath().c_str());

    auto result = signal(SIGINT, DaemonApplication<ThreadMainType>::signalHandler) != SIG_ERR;
    result &= signal(SIGQUIT, DaemonApplication<ThreadMainType>::signalHandler) != SIG_ERR;
    result &= signal(SIGTERM, DaemonApplication<ThreadMainType>::signalHandler) != SIG_ERR;
    // signal(SIGHUP, handleTerminateSignal);

    return result;
}

template<class ThreadMainType>
void DaemonApplication<ThreadMainType>::runMainLoop()
{
    if (_console)
    {
        readLine();
    }
    else
    {
        if (pipe2(_pipeDescriptors, O_NONBLOCK) != 0)
            return;

        int result;
        do
        {
            pollfd fds{.fd = _pipeDescriptors[0], .events = POLLIN, .revents = 0};
            result = poll(&fds, 1,  -1);
        }
        while (result < 0 && EINTR == errno);
    }
}

template<class ThreadMainType>
void DaemonApplication<ThreadMainType>::exitMainLoop()
{
    if (_console)
        return;

    ::close(_pipeDescriptors[0]);
    ::close(_pipeDescriptors[1]);
}

template<class ThreadMainType>
inline ThreadMainType *DaemonApplication<ThreadMainType>::threadMain()
{
    return _threadMain;
}

template<class ThreadMainType>
bool DaemonApplication<ThreadMainType>::console()
{
    return _console;
}

template<class ThreadMainType>
int DaemonApplication<ThreadMainType>::logLevel()
{
    return _logLevel;
}

template<class ThreadMainType>
bool DaemonApplication<ThreadMainType>::parseArguments()
{
    constexpr char PARAMETER_NAME_CONSOLE[] = "--console";
    constexpr char PARAMETER_NAME_INSTALL[] = "--install";
    constexpr char PARAMETER_NAME_LOG[] = "--log";
    constexpr char PARAMETER_NAME_UNINSTALL[] = "--uninstall";

    _console = appInfo().commandLine().optionExist(PARAMETER_NAME_CONSOLE);

    if (appInfo().commandLine().optionExist(PARAMETER_NAME_LOG))
    {
        const auto logLevelString = appInfo().commandLine().optionValue(PARAMETER_NAME_LOG);
        if (!logLevelString.empty() && logLevelString.find_first_not_of("0123456789") == std::string::npos)
            _logLevel = std::stoi(logLevelString);
    }

    if (appInfo().commandLine().optionExist(PARAMETER_NAME_INSTALL))
    {
        if (appInfo().commandLine().tokenCount() != 2)
            return false;

        _install = true;
    }
    else if (appInfo().commandLine().optionExist(PARAMETER_NAME_UNINSTALL))
    {
        if (appInfo().commandLine().tokenCount() != 2)
            return false;

        _uninstall = true;
    }

    return true;
}

template<class ThreadMainType>
void DaemonApplication<ThreadMainType>::signalHandler(int signal)
{
    writeLog(Message16, signal);

    DaemonUtils::daemonSetRunning(_serviceName, false);

    if (!_instance)
        return;

    _instance->_isRunning = false;
    _instance->exitMainLoop();

    if (_instance->_console)
    {
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 0);
        ::close(fd);
        std::cout << std::endl;
    }
}

}

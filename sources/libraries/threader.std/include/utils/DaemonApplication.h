#pragma once

#include "CommandLineParser.h"
#include "DaemonApplicationInfo.h"
#include "DaemonUtils.h"
#include "DateTimeUtils.h"
#include "LogMessagesTemplates.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"

#include "Log.h"

#include <csignal>
#include <iostream>

#include <fcntl.h>

#ifdef linux
#include <poll.h>
#include <systemd/sd-daemon.h>
#endif

#ifdef WIN32
#include "ServiceBase.h"
#include "ServiceInstaller.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Threader::Threads {

using namespace Threader::Utils;

void readLine()
{
    std::string line;
    std::getline(std::cin, line);
    writeLog(Message16, -1);
}

template<class ThreadMainType>
class DaemonApplication
{
public:
    explicit DaemonApplication(int argc,
                               char *argv[],
                               const std::string &description = std::string(),
                               const std::string &version = std::string(),
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

    uint32_t installDaemon();
    uint32_t uninstallDaemon();

private:
    static DaemonApplication *_instance;
    ThreadMainType *_threadMain;
    bool _isRunning = true;
    int _argc;
    std::string _description;
    static std::string _serviceName;
    static std::string _serviceExecArgs;

#ifdef linux
    Descriptor _pipeDescriptors[2];
#endif
#ifdef WIN32
    Descriptor _event;
    ServiceThreader *_service;
#endif

    bool _console;
    bool _systemd;
    bool _install;
    bool _uninstall;
    int _logLevel;
    std::string _version;
};

template<class ThreadMainType>
std::string DaemonApplication<ThreadMainType>::_serviceName = std::string();

template<class ThreadMainType>
std::string DaemonApplication<ThreadMainType>::_serviceExecArgs = std::string();

template<class ThreadMainType>
DaemonApplication<ThreadMainType> *DaemonApplication<ThreadMainType>::_instance = nullptr;

template<class ThreadMainType>
DaemonApplication<ThreadMainType>::DaemonApplication(int argc,
                                                     char *argv[],
                                                     const std::string &description,
                                                     const std::string &version,
                                                     const std::string &serviceName)
    : _threadMain(nullptr)
    , _argc(argc)
    , _description(description)
    #ifdef linux
    , _pipeDescriptors{INVALID_DESCRIPTOR, INVALID_DESCRIPTOR}
    #endif
    #ifdef WIN32
    , _event(INVALID_DESCRIPTOR)
    , _service(nullptr)
    #endif
    , _console(false)
    , _systemd(false)
    , _install(false)
    , _uninstall(false)
    , _logLevel(9)
    , _version(version)
{
    _serviceName = serviceName;
    _instance = this;
    _console = false;

    appInfo().setArguments(argc, argv);
}

template<class ThreadMainType>
int DaemonApplication<ThreadMainType>::run()
{
#ifdef linux
    _systemd = 0 < sd_notify(0, format("STATUS=Service %s starting...", _serviceName.c_str()).c_str());
#endif

    if (!initialize())
        return -1;

    _threadMain = new ThreadMainType(_serviceName, _description);

#ifdef WIN32
    _service = nullptr;
    if (!_console || _install || _uninstall)
    {
        _service = new ServiceThreader(_threadMain,
                                       _serviceName,
                                       _description,
                                       SERVICE_AUTO_START,
                                       SERVICE_ERROR_NORMAL,
                                       SERVICE_ACCEPT_STOP);
    }
#endif

    if (_install)
        return installDaemon();

    if (_uninstall)
        return uninstallDaemon();

    if (DaemonUtils::daemonIsRunning(_serviceName))
    {
        writeLog(Message17, _serviceName.c_str());
    }

    DaemonUtils::daemonSetRunning(_serviceName, true);

#ifdef WIN32
    if (_console)
#endif
        _threadMain->startThread();

#ifdef linux
    if (_systemd)
        _systemd = 0 < sd_notify(0, format("READY=1\nSTATUS=Service %s is ready", _serviceName.c_str()).c_str());
#endif

    runMainLoop();

#ifdef linux
    if (_systemd)
        _systemd = 0 < sd_notify(0, format("STOPPING=1\nSTATUS=Finising service %s...", _serviceName.c_str()).c_str());
#endif

    _threadMain->postTerminateEvent();
    while (!_threadMain->isThreadFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    DaemonUtils::daemonSetRunning(_serviceName, false);

    stopLogSubsystem();

#ifdef linux
    if (_systemd)
        _systemd = 0 < sd_notify(0, format("STATUS=Service %s finished", _serviceName.c_str()).c_str());
#endif

    return 0;
}

template<class ThreadMainType>
bool DaemonApplication<ThreadMainType>::initialize()
{
#ifdef linux
    setErrorSignalHandler(appInfo().commandLine().binaryFileName().c_str());

    setlocale(LC_ALL, "ru_RU.UTF-8");
    setlocale(LC_NUMERIC, "C");

    std::string line = "Нажмите Enter или Ctrl+C для завершения программы...\r\n";
#endif

#ifdef WIN32
    constexpr char MESSAGE_START_HINT[] = "Нажмите Enter для завершения программы...\r\n";
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    auto line = MESSAGE_START_HINT;
#endif
    chdir(appInfo().binaryPath().c_str());

    if (appInfo().commandLine().tokenCount() < 1)
    {
#ifdef linux
        if (_systemd)
            _systemd = 0 < sd_notify(0, "ERRNO=1\nSTATUS=Arguments parse error");
#endif
        return false;
    }

    if (!parseArguments())
    {
#ifdef linux
        if (_systemd)
            _systemd = 0 < sd_notify(0, "ERRNO=2\nSTATUS=Arguments parse error");
#endif
        return false;
    }

    if (_console)
        std::cout << line;

    bool clearHistory = !_install && !_uninstall;
    if (!clearHistory)
        _console = true;

    startLogSubsystem(appInfo().binaryPath() + appInfo().binaryFileName(),
                      nullptr, _console, _logLevel, clearHistory);

    if (_serviceName.empty())
    {
        _serviceName = appInfo().binaryFileName();
#ifdef WIN32
        if (_serviceName.size() > 3)
            _serviceName.resize(_serviceName.length() - 4);
#endif
    }

#ifdef linux
    _systemd = 0 < sd_notify(0, format("STATUS=Service %s starting...", _serviceName.c_str()).c_str());

    auto result = signal(SIGINT, DaemonApplication<ThreadMainType>::signalHandler) != SIG_ERR;
    result &= signal(SIGTERM, DaemonApplication<ThreadMainType>::signalHandler) != SIG_ERR;
    result &= signal(SIGQUIT, DaemonApplication<ThreadMainType>::signalHandler) != SIG_ERR;
    if (_systemd && !result)
        _systemd = 0 < sd_notify(0, "ERRNO=3\nSTATUS=Signal handler error");
    return result;
#endif

#ifdef WIN32
    return true;
#endif

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
#ifdef linux
        if (pipe2(_pipeDescriptors, O_NONBLOCK) != 0)
            return;

        int result;
        do
        {
            pollfd fds{.fd = _pipeDescriptors[0], .events = POLLIN, .revents = 0};
            result = poll(&fds, 1,  -1);
        }
        while (result < 0 && EINTR == errno);
#endif
#ifdef WIN32
        _service->Run();
#endif
    }

}

template<class ThreadMainType>
void DaemonApplication<ThreadMainType>::exitMainLoop()
{
    if (_console)
        return;

#ifdef linux
    ::close(_pipeDescriptors[0]);
    ::close(_pipeDescriptors[1]);
#endif

#ifdef WIN32
    CloseHandle(_event);
#endif
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

constexpr char OPTION_NAME_CONSOLE_LONG[] = "--console";
constexpr char OPTION_NAME_CONSOLE_SHORT[] = "-c";

constexpr char OPTION_NAME_HELP_LONG[] = "--help";
constexpr char OPTION_NAME_HELP_SHORT[] = "-h";

constexpr char OPTION_NAME_LOG_LONG[] = "--log-level";
constexpr char OPTION_NAME_LOG_SHORT[] = "-l";

constexpr char OPTION_NAME_VERSION_LONG[] = "--version";
constexpr char OPTION_NAME_VERSION_SHORT[] = "-v";

constexpr char COMMAND_NAME_INSTALL[] = "install";
constexpr char COMMAND_NAME_UNINSTALL[] = "uninstall";

constexpr char ARG_NAME_NAME_LONG[] = "--name";
constexpr char ARG_NAME_NAME_SHORT[] = "-n";

constexpr char ARG_EXEC_START_NAME_LONG[] = "--exec-args";
constexpr char ARG_EXEC_START_NAME_SHORT[] = "-e";

template<class ThreadMainType>
bool DaemonApplication<ThreadMainType>::parseArguments()
{

    COMM_OPTION("Вывод протокола работы в терминал", OPTION_NAME_CONSOLE_LONG,
               OPTION_NAME_CONSOLE_SHORT, false, false);

    COMM_OPTION("Вывод справки", OPTION_NAME_HELP_LONG,
               OPTION_NAME_HELP_SHORT, false, false);

    COMM_OPTION("Установка уровня протоколирования [-1..9]", OPTION_NAME_LOG_LONG,
               OPTION_NAME_LOG_SHORT, true, false);

    COMM_OPTION("Вывод версии и завершение приложения",OPTION_NAME_VERSION_LONG,
               OPTION_NAME_VERSION_SHORT, false, false);

    OptionList list = {
        CommandLineOption("Идентификатор службы в systemd",
        ARG_NAME_NAME_LONG, ARG_NAME_NAME_SHORT, true, false),
        CommandLineOption("Аргументы командной строки запуска службы",
        ARG_EXEC_START_NAME_LONG, ARG_EXEC_START_NAME_SHORT, true, false)
    };
    COMM_COMMAND("Регистрация службы и завершение приложения", COMMAND_NAME_INSTALL, list);

    list = {
        CommandLineOption("Идентификатор службы в systemd",
        ARG_NAME_NAME_LONG, ARG_NAME_NAME_SHORT, true, false),
    };
    COMM_COMMAND("Снятие регистрации службы и завершение приложения", COMMAND_NAME_UNINSTALL, list);

    std::string error{};
    bool result = appInfo().commandLine().parse(&error);

    if (!result || appInfo().commandLine().hasParsedOption(OPTION_NAME_HELP_LONG))
    {
        if (!error.empty())
            std::cout << "Ошибка разбора командной строки: " << error << std::endl;

        std::cout << appInfo().commandLine().help();
        exit(1);
    }

    bool showVersion = appInfo().commandLine().hasParsedOption(OPTION_NAME_VERSION_LONG);
    if (showVersion)
    {
        std::cout << "Version [" << _version << "]" << std::endl;
        exit(0);
    }

    _console = appInfo().commandLine().hasParsedOption(OPTION_NAME_CONSOLE_LONG);

    std::string logLevelString;
    if (appInfo().commandLine().hasParsedOption(OPTION_NAME_LOG_LONG, &logLevelString))
    {
        if (!logLevelString.empty() && logLevelString.find_first_not_of("0123456789") == std::string::npos)
            _logLevel = std::stoi(logLevelString);
    }

    const auto command = appInfo().commandLine().parsedCommand().command();

    if (command == COMMAND_NAME_INSTALL)
    {
        _install = true;
        auto optionName = appInfo().commandLine().parsedCommand().findOption(ARG_NAME_NAME_SHORT);
        if (optionName)
            _serviceName = optionName->value();

        auto optionExecArgs = appInfo().commandLine().parsedCommand().findOption(ARG_EXEC_START_NAME_SHORT);
        if (optionExecArgs)
            _serviceExecArgs = optionExecArgs->value();
    }
    else if (command == COMMAND_NAME_UNINSTALL)
    {
        _uninstall = true;
        auto optionName = appInfo().commandLine().parsedCommand().findOption(ARG_NAME_NAME_SHORT);
        if (optionName)
            _serviceName = optionName->value();
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

template<class ThreadMainType>
uint32_t DaemonApplication<ThreadMainType>::installDaemon()
{
    writeLog(Message50, _serviceName.c_str());

    std::string resultString;
#ifdef linux
    auto applicationBinaryFile = appInfo().binaryPath() + appInfo().binaryFileName();
    auto registered = DaemonUtils::daemonRegister(_serviceName,
                                                  applicationBinaryFile,
                                                  _serviceExecArgs,
                                                  _description,
                                                  &resultString);
#endif

#ifdef WIN32
    bool registered = true;
    if (_service)
        registered = ServiceInstaller::Install(*_service, resultString);
#endif
    if (registered)
    {
        writeLog(Message51, _serviceName.c_str());
        stopLogSubsystem();
        return 0;
    }

    writeLog(Message52, _serviceName.c_str(), resultString.c_str());
    stopLogSubsystem();
    return -1;

}

template<class ThreadMainType>
uint32_t DaemonApplication<ThreadMainType>::uninstallDaemon()
{
    writeLog(Message60, _serviceName.c_str());

    std::string resultString;
#ifdef linux
    auto unregistered = DaemonUtils::daemonUnregister(_serviceName,
                                                      &resultString);
#endif
#ifdef WIN32
    bool unregistered = true;
    if (_service)
        unregistered = ServiceInstaller::Uninstall(*_service, resultString);
#endif

    if (unregistered)
    {
        writeLog(Message61, _serviceName.c_str());
        stopLogSubsystem();
        return 0;
    }

    writeLog(Message62, _serviceName.c_str(), resultString.c_str());
    stopLogSubsystem();
    return -1;

}

}

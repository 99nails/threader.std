#pragma once

#include "CommandLineParser.h"
#include "DaemonUtils.h"
#include "DateTimeUtils.h"
#include "FileUtils.h"
#include "SignalHandler.h"
// #include "StdStringUtils.h"

#include "ThreadLog.h"

#include <csignal>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>


using namespace Threader::Threads;

static bool isRunning = true;
static bool console = false;
static bool install = false;
static int logLlevel = 9;
static bool uninstall = false;

void handleTerminateSignal(int signum)
{
    isRunning = false;

    // закрытие stdin в случае консольного вывода
    if (console)
    {
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 0);
        close(fd);
        std::cout << std::endl;
    }

    ThreadBase::writeLog(Message15, signum);
}

void setTerminateSignalHandlers()
{
    signal(SIGINT, handleTerminateSignal);
    signal(SIGTERM, handleTerminateSignal);
    // signal(SIGHUP, handleTerminateSignal);
}

void readLine()
{
    std::string line;
    std::getline(std::cin, line);
}

bool parseArguments(int argc,
                    char *argv[],
                    bool &console,
                    int &level,
                    bool &install,
                    bool &uninstall)
{
    bool isValid = true;

    constexpr char PARAMETER_NAME_CONSOLE[] = "--console";
    constexpr char PARAMETER_NAME_INSTALL[] = "--install";
    constexpr char PARAMETER_NAME_LOG[] = "--log";
    constexpr char PARAMETER_NAME_UNINSTALL[] = "--uninstall";

    CommandLineParser commandLine(argc, argv);

    console = commandLine.optionExist(PARAMETER_NAME_CONSOLE);

    if (commandLine.optionExist(PARAMETER_NAME_LOG))
    {
        const auto logLevelString = commandLine.optionValue(PARAMETER_NAME_LOG);
        if (!logLevelString.empty() && logLevelString.find_first_not_of("0123456789") == std::string::npos)
            level = std::stoi(logLevelString);
    }

    if (commandLine.optionExist(PARAMETER_NAME_INSTALL))
    {
        if (argc != 2)
            return false;

        install = true;
    }
    else if (commandLine.optionExist(PARAMETER_NAME_UNINSTALL))
    {
        if (argc != 2)
            return false;

        uninstall = true;
    }


    return true;
}


template<class ThreadMainType>
int AppMain(int argc,
            char *argv[],
            const std::string &serviceName = std::string(),
            const std::string &description = std::string())
{
    // (void)argc;
#ifdef linux
    setErrorSignalHandler(argv[0]);
#endif

#ifdef WIN
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251("Нажмите Enter для завершения программы...\r\n");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    setlocale(LC_NUMERIC, "C");
    std::cout << "Нажмите Enter или Ctrl+C для завершения программы...\r\n";
#endif
    parseArguments(argc, argv, console, logLlevel, install, uninstall);

    startLogSubsystem(argv[0], nullptr, console, logLlevel);

    auto binaryFileName = FileUtils::extractFileName(argv[0]);

    if (install)
    {
        auto registered = DaemonUtils::daemonRegister((serviceName.empty()) ? binaryFileName : serviceName,
                                                      argv[0],
                                                      description);
        return (registered) ? 0 : -1;
    }
    else if (uninstall)
    {
        auto binaryFileName = FileUtils::extractFileName(argv[0]);
        auto unregistered = DaemonUtils::daemonUnregister((serviceName.empty()) ? binaryFileName : serviceName);
        return (unregistered) ? 0 : -1;
    }

    if (DaemonUtils::daemonIsRunning(serviceName))
    {
        ThreadBase::writeLog(Message16, serviceName.c_str());
    }

    DaemonUtils::daemonSetRunning(serviceName, true);

    auto threadMain = new ThreadMainType(binaryFileName);
    threadMain->start();

    if (console)
    {
        readLine();
    }
    else
    {
#ifdef linux
        setTerminateSignalHandlers();
#endif
        while (isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    threadMain->postTerminateEvent();
    while (!threadMain->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    DaemonUtils::daemonSetRunning(serviceName, false);

    stopLogSubsystem();

    return 0;
}

#pragma once

#include "CommandLineParser.h"
#include "DateTimeUtils.h"
#include "FileUtils.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"

#include "ThreadLog.h"

#include <csignal>
#include <iostream>

using namespace Threader::Threads;

static bool isRunning = true;
static bool console = false;
static int logLlevel = 9;

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

void parseArguments(int argc,
                      char *argv[],
                      bool &console,
                      int &level)
{
    constexpr char PARAMETER_NAME_CONSOLE[] = "--console";
    constexpr char PARAMETER_NAME_LOG[] = "--log";

    CommandLineParser commandLine(argc, argv);

    console = commandLine.optionExist(PARAMETER_NAME_CONSOLE);

    if (commandLine.optionExist(PARAMETER_NAME_LOG))
    {
        const auto logLevelString = commandLine.optionValue(PARAMETER_NAME_LOG);
        if (!logLevelString.empty() && logLevelString.find_first_not_of("0123456789") == std::string::npos)
            level = std::stoi(logLevelString);
    }
}


template<class ThreadMainType>
int AppMain(int argc, char *argv[])
{
    (void)argc;
#ifdef linux
    setErrorSignalHandler(argv[0]);
    setTerminateSignalHandlers();
#endif

#ifdef WIN
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251("Нажмите Enter для завершения программы...\r\n");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    setlocale(LC_NUMERIC, "C");
    std::cout << "Нажмите Enter или Ctrl+C для завершения программы...\r\n";
#endif
    parseArguments(argc, argv, console, logLlevel);

    startLogSubsystem(argv[0], nullptr, console, logLlevel);

    auto applicationName = FileUtils::extractFileName(argv[0]);

    auto threadMain = new ThreadMainType(applicationName);
    threadMain->start();

    if (console)
    {
        readLine();
    }
    else
    {
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

    stopLogSubsystem();

    return 0;
}

#pragma once

#include "DateTimeUtils.h"
#include "FileUtils.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"

#include "ThreadLog.h"

#include <iostream>

using namespace Threader::Threads;

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

    for (int i = 1; i < argc; i++)
    {
        const auto parameter = StdStringUtils::toLower(argv[i]);
        if (parameter == PARAMETER_NAME_CONSOLE)
        {
            console = true;
        }
        if (parameter == PARAMETER_NAME_LOG)
        {
            if (i < argc + 1)
            {
                i++;
                std::string levelString = argv[i];
                if (levelString.find_first_not_of("0123456789") == std::string::npos)
                    level = std::stoi(argv[i]);
            }
        }
    }
}


template<class ThreadMainType>
int AppMain(int argc, char *argv[])
{
    (void)argc;
#ifdef linux
    setSignalHandler(argv[0]);
#endif

#ifdef WIN
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251("Нажмите Enter для завершения программы...\r\n");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    setlocale(LC_NUMERIC, "C");
    std::cout << "Нажмите Enter для завершения программы...\r\n";
#endif
    bool console = false;
    int level = 9;
    parseArguments(argc, argv, console, level);

    startLogSubsystem(argv[0], nullptr, console, level);

    auto applicationName = FileUtils::extractFileName(argv[0]);

    auto threadMain = new ThreadMainType(applicationName);
    threadMain->start();

    readLine();

    threadMain->postTerminateEvent();
    while (!threadMain->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    stopLogSubsystem();

    return 0;
}

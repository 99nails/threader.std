#pragma once

#include "DateTimeUtils.h"
#include "SignalHandler.h"

#include "ThreadLog.h"

#include <iostream>

using namespace Threader::Threads;

void readStr()
{
    std::string ch;
    std::getline(std::cin, ch);
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
    std::cout << "Press Enter to terminate program...\r\n";
#endif

    startLogSubsystem(argv[0]);

    auto threadMain = new ThreadMainType();
    threadMain->start();

    readStr();

    threadMain->postTerminateEvent();
    while (!threadMain->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    stopLogSubsystem();
}

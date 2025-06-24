#include "DateTimeUtils.h"
#include "StdStringUtils.h"

#include "Example3Common.h"
#include "ThreadLog.h"
#include "ThreadPing.h"
#include "ThreadPong.h"

#include <iostream>

using namespace Threader::Threads;

void readStr()
{
    std::string ch;
    std::getline(std::cin, ch);
}

int main(int argc, char *argv[])
{
#ifdef WIN32
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251("Нажмите Enter для завершения программы...\r\n");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    auto line = "Нажмите Enter для завершения программы...\r\n";
#endif

    std::cout << line;

    auto threadLog = new ThreadLog(argv[0]);
    threadLog->start();

    ThreadBase::writeLog(Message10);

    auto threadPing = new ThreadPing();
    auto threadPong = new ThreadPong();

    threadPing->setThreadPong(threadPong);
    threadPong->setThreadPing(threadPing);

    threadPing->start();
    threadPong->start();

    ThreadBase::writeLog(Message11);

    readStr();

    ThreadBase::writeLog(Message12);

    threadPing->postTerminateEvent();
    threadPong->postTerminateEvent();

    DateTimeUtils::sleepMilliseconds(1);

    if (threadPing->wasStarted())
    {
        while (!threadPing->isFinished())
        {
            DateTimeUtils::sleepMilliseconds(1000);
        }
    }

    if (threadPong->wasStarted())
    {
        while (!threadPong->isFinished())
        {
            DateTimeUtils::sleepMilliseconds(1000);
        }
    }

    ThreadBase::writeLog(Message13);

    if (threadLog->wasStarted())
    {
        threadLog->postTerminateEvent();
        while (!threadLog->isFinished())
        {
            DateTimeUtils::sleepMilliseconds(100);
        }
    }

    delete threadPing;
    delete threadPong;
    delete threadLog;
}

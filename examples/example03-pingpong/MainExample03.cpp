#include "DateTimeUtils.h"
#include "StdStringUtils.h"

#include "Example03Common.h"
#include "ThreadLog.h"
#include "ThreadPing.h"
#include "ThreadPong.h"

#include <iostream>

using namespace Threader::Threads;

void readLine()
{
    std::string ch;
    std::getline(std::cin, ch);
}

constexpr char MESSAGE_START_HINT[] = "Нажмите Enter для завершения программы...\r\n";

int main(int argc, char *argv[])
{

#ifdef WIN32
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251(MESSAGE_START_HINT);
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    auto line = MESSAGE_START_HINT;
#endif

    std::cout << line;

    auto threadLog = new ThreadLog(argv[0], nullptr, true);
    threadLog->startThread();

    ThreadBase::writeLog(Message110);

    auto threadPing = new ThreadPing();
    auto threadPong = new ThreadPong();

    threadPing->setThreadPong(threadPong);
    threadPong->setThreadPing(threadPing);

    threadPing->startThread();
    threadPong->startThread();

    ThreadBase::writeLog(Message111);

    readLine();

    ThreadBase::writeLog(Message112);

    threadPing->postTerminateEvent();
    threadPong->postTerminateEvent();

    DateTimeUtils::sleepMilliseconds(1);

    if (threadPing->wasStarted())
    {
        while (!threadPing->isThreadFinished())
        {
            DateTimeUtils::sleepMilliseconds(1000);
        }
    }

    if (threadPong->wasStarted())
    {
        while (!threadPong->isThreadFinished())
        {
            DateTimeUtils::sleepMilliseconds(1000);
        }
    }

    ThreadBase::writeLog(Message113);

    if (threadLog->wasStarted())
    {
        threadLog->postTerminateEvent();
        while (!threadLog->isThreadFinished())
        {
            DateTimeUtils::sleepMilliseconds(100);
        }
    }

    delete threadPing;
    delete threadPong;
    delete threadLog;
}

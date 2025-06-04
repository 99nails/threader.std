#include "DateTimeUtils.h"
#include "Example3Common.h"
#include "ThreadLog.h"
#include "ThreadPing.h"
#include "ThreadPong.h"

#include "Logger.h"

#include <iostream>

using namespace Threader::Threads;

void readStr()
{
    std::string ch;
    std::getline(std::cin, ch);
}

int main(int argc, char *argv[])
{
    std::cout << "Press Enter to terminate program...\r\n";

    // Logger logger(true,
    //               // ".",
    //               // "logfile",
    //               argv[0],
    //               9,
    //               0xFF);

    // for (int i = 0; i < 20; i++)
    // {
    //     logger.write(DateTimeUtils::msecsSinceEpochLocal(),
    //                  0,
    //                  9,
    //                  MessageType::Message,
    //                  "Common subsystem",
    //                  -1,
    //                  "Test message " + std::to_string(i));
    //     logger.flush();
    //     DateTimeUtils::sleepMilliseconds(50);
    // }

    auto threadLog = new ThreadLog(argv[0]);
    threadLog->start();

    // ThreadLog::writeLog("Starting threads...");
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

    while (!threadPing->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(1000);
    }

    while (!threadPong->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(1000);
    }

    ThreadBase::writeLog(Message13);

    threadLog->postTerminateEvent();
    while (!threadLog->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    delete threadPing;
    delete threadPong;
    delete threadLog;

    // readStr();
}

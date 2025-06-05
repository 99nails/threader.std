// socat -d -d pty,rawer,echo=0 pty,rawer,echo=0 &

#include "DateTimeUtils.h"
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
    std::cout << "Press Enter to terminate program...\r\n";

    auto threadLog = new ThreadLog(argv[0]);
    threadLog->start();

    auto threadPing = new ThreadPing();
    threadPing->start();

    auto threadPong = new ThreadPong();
    threadPong->start();

    readStr();

    threadPing->postTerminateEvent();
    threadPong->postTerminateEvent();

    while (!threadPing->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    while (!threadPong->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    threadLog->postTerminateEvent();
    while (!threadLog->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    delete threadPong;
    delete threadPing;
    delete threadLog;
}

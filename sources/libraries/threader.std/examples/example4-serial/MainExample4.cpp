// use in Linux
// socat -d -d pty,rawer,echo=0 pty,rawer,echo=0 &
// use NullModem Emulator in Windows

#include "DateTimeUtils.h"
#include "SignalHandler.h"

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

int main(int argc, char *argv[])
{
    (void)argc;
#ifdef linux
    setErrorSignalHandler(argv[0]);
#endif

#ifdef WIN32
    setlocale(LC_ALL, "russian_Russia.1251");
    std::cout << StdStringUtils::ConvertUTF8ToCP1251("Press Enter to terminate program...\r\n");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    std::cout << "Press Enter to terminate program...\r\n";
#endif

    startLogSubsystem(argv[0]);

    auto threadPing = new ThreadPing(
            #ifdef WIN32
                "COM4"
            #else
                "/dev/pts/1"
            #endif
                );
    threadPing->start();

    auto threadPong = new ThreadPong(
            #ifdef WIN32
                "COM5"
            #else
                "/dev/pts/0"
            #endif
                );
    threadPong->start();

    readLine();

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

    delete threadPong;
    delete threadPing;

    stopLogSubsystem();
}

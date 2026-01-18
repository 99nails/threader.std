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

constexpr char MESSAGE_START_HINT[] = "Нажмите Enter для завершения программы...\r\n";

int main(int argc, char *argv[])
{
    (void)argc;
#ifdef linux
    setErrorSignalHandler(argv[0]);
#endif

#ifdef WIN32
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251(MESSAGE_START_HINT);
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    auto line = MESSAGE_START_HINT;
#endif

    std::cout << line;

    startLogSubsystem(argv[0], nullptr, true);

    auto threadPing = new ThreadPing(
            #ifdef WIN32
                "COM4"
            #else
                "/dev/pts/1"
            #endif
                );
    threadPing->startThread();

    auto threadPong = new ThreadPong(
            #ifdef WIN32
                "COM5"
            #else
                "/dev/pts/0"
            #endif
                );
    threadPong->startThread();

    readLine();

    threadPing->postTerminateEvent();
    threadPong->postTerminateEvent();

    while (!threadPing->isThreadFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    while (!threadPong->isThreadFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    delete threadPong;
    delete threadPing;

    stopLogSubsystem();
}

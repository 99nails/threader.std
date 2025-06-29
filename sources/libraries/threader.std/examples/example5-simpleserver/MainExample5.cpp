#include "DateTimeUtils.h"
#include "StdStringUtils.h"
#include "SignalHandler.h"

#include "ThreadLog.h"

#include <iostream>

using namespace Threader::Threads;

void readStr()
{
    std::string ch;
    std::getline(std::cin, ch);
}

int main(int argc, char *argv[])
{
    (void)argc;
#ifdef linux
    setSignalHandler(argv[0]);
#endif

#ifdef WIN
    setlocale(LC_ALL, "russian_Russia.1251");
    auto line = StdStringUtils::ConvertUTF8ToCP1251("Press Enter to terminate program...\r\n");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
    std::cout << "Press Enter to terminate program...\r\n";
#endif

    auto threadLog = new ThreadLog(argv[0]);
    threadLog->start();

    readStr();

    threadLog->postTerminateEvent();
    while (!threadLog->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

}

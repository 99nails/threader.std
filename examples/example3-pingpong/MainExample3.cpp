#include "DateTimeUtils.h"
#include "StdStringUtils.h"

#include "Example3Common.h"
#include "ThreadLog.h"
#include "ThreadPing.h"
#include "ThreadPong.h"

#ifdef QT_SUPPORT
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#endif

#include <iostream>

using namespace Threader::Threads;

void readLine()
{
    std::string ch;
    std::getline(std::cin, ch);
#ifdef QT_SUPPORT
    QCoreApplication::exit(0);
#endif
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

#ifdef QT_SUPPORT
    QCoreApplication a(argc, argv);

    QtConcurrent::run(readLine);
#endif

    std::cout << line;

    auto threadLog = new ThreadLog(argv[0], nullptr, true);
    threadLog->start();

    ThreadBase::writeLog(Message110);

    auto threadPing = new ThreadPing();
    auto threadPong = new ThreadPong();

    threadPing->setThreadPong(threadPong);
    threadPong->setThreadPing(threadPing);

    threadPing->start();
    threadPong->start();

    ThreadBase::writeLog(Message111);

#ifdef QT_SUPPORT
    a.exec();
#else
    readLine();
#endif

    ThreadBase::writeLog(Message112);

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

    ThreadBase::writeLog(Message113);

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

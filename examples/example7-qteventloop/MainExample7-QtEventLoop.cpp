#include "ThreadMainQtEventLoop.h"

#include <DaemonApplicationQt.h>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>

// #include <iostream>

// void readLine()
// {
//     std::string line;
//     std::getline(std::cin, line);
//     QCoreApplication::exit(0);
// }

int main(int argc, char *argv[])
{
    DaemonApplicationQt<ThreadMainQtEventLoop> application(argc, argv, "Клиент сервера времени GPS");
    return application.run();
    /*
    QCoreApplication a(argc, argv);

    QtConcurrent::run(readLine);

    auto thread = new ThreadMainQtEventLoop("example7", "Работа с QtEventLoop");

    thread->startThread();

    a.exec();

    thread->postTerminateEvent();
    while (!thread->isFinished())
    {
        QThread::msleep(100);
    }
    return 0;
*/
}

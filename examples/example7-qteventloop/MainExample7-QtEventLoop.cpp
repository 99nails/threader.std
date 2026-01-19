#include "ThreadMainQtEventLoop.h"

#include <DaemonApplicationQt.h>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    DaemonApplicationQt<ThreadMainQtEventLoop> application(argc, argv, "Демонстрация работы QtEventLoop");
    return application.run();
}

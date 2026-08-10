#include "MainWindowMessageBridge.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowMessageBridge w;
    w.show();
    return a.exec();
}

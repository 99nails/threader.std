#include "MainWindowUdevListener.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowUdevListener w;
    w.show();
    return a.exec();
}

#include "MainWindowIniFile.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowIniFile w;
    w.show();
    return a.exec();
}

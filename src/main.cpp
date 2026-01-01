#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("双设备采集系统 - DPS-16 & DTS-48");
    w.show();

    return a.exec();
}

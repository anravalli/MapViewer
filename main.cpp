#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // setlocale (LC_ALL, "c");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

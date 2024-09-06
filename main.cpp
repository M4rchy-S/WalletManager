#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowIcon(QIcon(":/main/main.ico"));
    w.setWindowTitle(QString("Desktop Wallet Manager"));

    w.show();

    return a.exec();
}



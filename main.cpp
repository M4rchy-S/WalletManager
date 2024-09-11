#include "mainwindow.h"

#include <QApplication>
#include <QSharedMemory>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //  Check for duplicate app running
    QSharedMemory sharedMemory;
    sharedMemory.setKey("EhJvNui6eUTZRskhzsza3qdaxGIqIjO2");

    if(!sharedMemory.create(1))
    {
        QMessageBox::warning(nullptr, "Error", "Program is already running");
        return 0;
    }

    w.setWindowIcon(QIcon(":/main/main.ico"));
    w.setWindowTitle(QString("Desktop Wallet Manager"));
    w.show();

    return a.exec();

}



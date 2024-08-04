#include "mainwindow.h"

#include <QApplication>

#include <eWallet.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    //std::cout << sendRequest() << std::endl;

    wlt::eWallet wallet;

    wallet.showRate();

    return a.exec();
}



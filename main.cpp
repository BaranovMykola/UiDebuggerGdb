#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try
    {

    MainWindow w;
    w.show();
    a.exec();
    }
    catch(std::exception exc)
    {
        qDebug() << exc.what();
    }

    return 0;
}

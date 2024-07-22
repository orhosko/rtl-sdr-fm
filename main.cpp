#include <QApplication>
#include "mainwindow.h"
#include "qwindowdefs.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return QApplication::exec();
}

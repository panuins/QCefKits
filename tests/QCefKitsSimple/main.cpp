#include "widget.h"
#include <QCefKits.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCefKitsSettings cefsettings(argc, argv);
//    cefsettings.backgroundColor = Qt::red;
//    cefsettings.logFile = "/dev/stdout";
//    cefsettings.multiThread = false;
    QCefKitsInit(argc, argv, cefsettings);

    Widget w;
    w.show();
    int ret = a.exec();

    QCefKitsShutdown();
    return ret;
}

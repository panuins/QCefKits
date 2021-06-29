#include "widget.h"
#include <QCefKits.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCefKitsInit(argc, argv, QCefKitsSettings());

    Widget w;
    w.show();
    int ret = a.exec();

    QCefKitsShutdown();
    return ret;
}

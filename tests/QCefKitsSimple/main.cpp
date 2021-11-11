#include "widget.h"
#include <QCefKits.h>
#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    //qputenv("QT_QPA_PLATFORMTHEME", "gtk3");
    //setenv("QT_QPA_PLATFORMTHEME","gtk3",1);
    //char envstr[] = "QT_QPA_PLATFORMTHEME=gtk3";
//    char envstr[] = "QT_STYLE_OVERRIDE=kvantum";
    //putenv(envstr);
    unsetenv("QT_QPA_PLATFORMTHEME");
    int ret = 0;
    {
        QApplication a(argc, argv);
        QCefKitsSettings cefsettings(argc, argv);
        QDir dir = a.applicationDirPath();
        cefsettings.userDataPath = dir.absoluteFilePath("userdata");
        cefsettings.cachePath = dir.absoluteFilePath("cache");
    //    cefsettings.backgroundColor = Qt::red;
    //    cefsettings.logFile = "/dev/stdout";
    //    cefsettings.multiThread = false;
        QCefKitsInit(argc, argv, cefsettings);

        Widget w;
        w.show();
        ret = a.exec();
        qDebug() << "after QApplication::exec";
    }

    QCefKitsShutdown();
    return ret;
}

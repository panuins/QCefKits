#include "MainWindow.h"
#include <QCefKits.h>
#include <QApplication>
#include <QDebug>
#include <QDir>

#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    setenv("IBUS_ENABLE_SYNC_MODE","1",1);
    unsetenv("QT_QPA_PLATFORMTHEME");
#endif
    int ret = 0;
    {
        QApplication a(argc, argv);
        QCefKitsSettings cefsettings(argc, argv);
        QDir dir = a.applicationDirPath();
        cefsettings.userDataPath = dir.absoluteFilePath("userdata");
        cefsettings.cachePath = dir.absoluteFilePath("cache");
        //cefsettings.no_sandbox = false;
    //    cefsettings.backgroundColor = Qt::red;
    //    cefsettings.logFile = "/dev/stdout";
    //    cefsettings.multiThread = false;
        QCefKitsInit(argc, argv, cefsettings);

        MainWindow w;
        w.show();
        ret = a.exec();
        qDebug() << "after QApplication::exec";
    }

    QCefKitsShutdown();
    return ret;
}

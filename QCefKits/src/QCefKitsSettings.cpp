/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "include/QCefKitsSettings.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <Windows.h>
static const char *subProcessFileName = "CefSubProcess.exe";
#elif defined(Q_OS_LINUX)
#include <unistd.h>
static const char *subProcessFileName = "CefSubProcess";
#endif

QCefKitsSettings::QCefKitsSettings(int /*argc*/, char **argv)
{
//    QString m_cachePath;
    backgroundColor = Qt::white;
    logLevel = QCefKits_LogLevel_Default;
    QDir appDirPath;
#ifdef Q_OS_WIN
    {
        wchar_t path[MAX_PATH];
        ::GetModuleFileNameW(nullptr, path, MAX_PATH);
        QString str = QString::fromWCharArray(path);
        appDirPath = QFileInfo(str).dir();
    }
#else
    {
        QFileInfo fileInfo(argv[0]);
        appDirPath = fileInfo.dir();
    }
#endif
    subProcessPath = appDirPath.absoluteFilePath(subProcessFileName);
//    subProcessPath = QDir(QApplication::applicationDirPath()).absoluteFilePath(subProcessFileName);
#ifdef Q_OS_WIN
    logFile = "nul";
#else
    logFile = "/dev/null";
#endif

    no_sandbox = true;
    multiThread = true;
}

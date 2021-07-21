/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef QCEFKITSSETTINGS_H
#define QCEFKITSSETTINGS_H

#include "QCefKitsGlobal.h"
#include <QColor>
#include <QMap>
#include <QString>

class QCEFKITS_EXPORT QCefKitsSettings
{
public:
    QCefKitsSettings(int argc, char **argv);

    QString cachePath;
    QString subProcessPath;
    QString acceptLanguageList;
    QString userAgent;
    QString userAgentProduction;
    QString userDataPath;
    QString logFile;
    QString locale;
    QString resourcesdirPath;
    QString localesDirPath;
    QColor backgroundColor;
    QCefKits_logLevel logLevel;
    bool multiThread;
    bool no_sandbox;
};

#endif // QCEFKITSSETTINGS_H

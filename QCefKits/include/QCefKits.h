/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef QCEFKITS_H
#define QCEFKITS_H

#include "QCefKitsGlobal.h"
#include "QCefKitsSettings.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#else
#endif

#ifdef Q_OS_WIN
bool QCEFKITS_EXPORT QCefKitsInit_win(HINSTANCE hInstance, const QCefKitsSettings& settings);
#endif
bool QCEFKITS_EXPORT QCefKitsInit(int argc, char** argv, const QCefKitsSettings& settings);

void QCEFKITS_EXPORT QCefKitsShutdown();

#endif // QCEFKITS_H

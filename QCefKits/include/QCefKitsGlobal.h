/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef QCEFWIDGETGLOBAL_H
#define QCEFWIDGETGLOBAL_H
#include <QtGlobal>
#ifdef QCEFKITS_LIBRARY
#define QCEFKITS_EXPORT Q_DECL_EXPORT
#else
#define QCEFKITS_EXPORT Q_DECL_IMPORT
#endif

enum QCefKits_logLevel
{
    QCefKits_LogLevel_Default,
    QCefKits_LogLevel_Verbose,
    QCefKits_LogLevel_Debug,
    QCefKits_LogLevel_Info,
    QCefKits_LogLevel_Warning,
    QCefKits_LogLevel_Error,
    QCefKits_LogLevel_Fatal,
    QCefKits_LogLevel_Disable,
};

#endif // QCEFWIDGETGLOBAL_H

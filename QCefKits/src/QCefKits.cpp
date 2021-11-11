/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "../include/QCefKits.h"
#include "ClientUtils.h"
#include "src/ClientAppBrowser.h"
#include "src/ClientAppRenderer.h"
#include "src/CefWebPage.h"
#include "../include/QCefWidget.h"
#include "src/QCefKits_internal.h"
#include "IPendingDialog.h"
#include <include/cef_app.h>
#include <include/cef_version.h>
#include <include/capi/cef_app_capi.h>
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QMutexLocker>
#ifdef Q_OS_LINUX
#include "X11Utils.h"
#endif


namespace QCefKits
{
static QMutex g_lock_pendingDialogs;
CefSettings g_cefSettings;
QTimer *g_timer = nullptr;
static std::queue<CefRefPtr<IPendingDialog> > g_pendingDialogs;
void pushPendingDialog(CefRefPtr<IPendingDialog> dialog)
{
    QMutexLocker locker(&g_lock_pendingDialogs);
    g_pendingDialogs.push(dialog);
}

CefRefPtr<IPendingDialog> popPendingDialog()
{
    QMutexLocker locker(&g_lock_pendingDialogs);
    CefRefPtr<IPendingDialog> ret = g_pendingDialogs.front();
    g_pendingDialogs.pop();
    return ret;
}

}

using QCefKits::g_cefSettings;
using QCefKits::g_timer;

static void copySettings(const QCefKitsSettings& settings)
{
    g_cefSettings.no_sandbox = settings.no_sandbox ? 1 : 0;
    g_cefSettings.background_color = static_cast<unsigned int>(settings.backgroundColor.rgba());
    g_cefSettings.log_severity = QCefKits::LogLevelToCefLogServerity(settings.logLevel);
    if (settings.multiThread)
    {
        g_cefSettings.multi_threaded_message_loop = 1;
        g_cefSettings.external_message_pump = 0;
    }
    else
    {
        g_cefSettings.multi_threaded_message_loop = 0;
        g_cefSettings.external_message_pump = 1;
    }
    if (!settings.subProcessPath.isEmpty())
    {
        CefString(&g_cefSettings.browser_subprocess_path) = settings.subProcessPath.toStdWString();
    }
    if (!settings.acceptLanguageList.isEmpty())
    {
        CefString(&g_cefSettings.accept_language_list) = settings.acceptLanguageList.toStdWString();
    }
    if (!settings.logFile.isEmpty())
    {
        CefString(&g_cefSettings.log_file) = settings.logFile.toStdWString();
    }
    if (!settings.locale.isEmpty())
    {
        CefString(&g_cefSettings.locale) = settings.locale.toStdWString();
    }
    if (!settings.userAgent.isEmpty())
    {
        CefString(&g_cefSettings.user_agent) = settings.userAgent.toStdWString();
    }
#if CEF_VERSION_MAJOR >= 90
    if (!settings.userAgentProduction.isEmpty())
    {
        CefString(&g_cefSettings.user_agent_product) = settings.userAgentProduction.toStdWString();
    }
#endif
    if (!settings.userDataPath.isEmpty())
    {
        CefString(&g_cefSettings.user_data_path) = settings.userDataPath.toStdWString();
    }
    if (!settings.cachePath.isEmpty())
    {
        CefString(&g_cefSettings.cache_path) = settings.cachePath.toStdWString();
    }
    if (!settings.resourcesdirPath.isEmpty())
    {
        CefString(&g_cefSettings.resources_dir_path) = settings.resourcesdirPath.toStdWString();
    }
    if (!settings.localesDirPath.isEmpty())
    {
        CefString(&g_cefSettings.locales_dir_path) = settings.localesDirPath.toStdWString();
    }
}

static bool initCef(CefMainArgs &main_args)
{
//    qRegisterMetaType<QCefWidget>("QCefWidget");
    qRegisterMetaType<QCefWidget *>("QCefWidget *");
//    qRegisterMetaType<CefWebPage>("CefWebPage");
    qRegisterMetaType<QSharedPointer<CefWebPage> >("QSharedPointer<CefWebPage>");
    g_timer = new QTimer();
//    if (g_cefSettings.external_message_pump)
    {
        g_timer->start(10);
    }
    QObject::connect(g_timer, &QTimer::timeout,
            g_timer, []()
    {
        if (g_cefSettings.external_message_pump)
        {
//            qDebug() << "CefDoMessageLoopWork";
            CefDoMessageLoopWork();
        }
        else
        {
//            qDebug() << "QTimer::timeout" << qApp->allWindows();
            while (QCefKits::g_pendingDialogs.size() > 0)
            {
                CefSetOSModalLoop(true);
                CefRefPtr<QCefKits::IPendingDialog> dialog = QCefKits::popPendingDialog();
                dialog->execPendingDialog();
                CefSetOSModalLoop(false);
            }
        }
    });
    CefRefPtr<CefApp> app(new QCefKits::ClientAppBrowser());
#ifdef Q_OS_LINUX
//    QCefKits::SetXErrorHandler();
#endif
    CefExecuteProcess(main_args, new QCefKits::ClientAppRenderer(), nullptr);
    return CefInitialize(main_args, g_cefSettings, app, nullptr);
}

#ifdef Q_OS_WIN
bool QCEFKITS_EXPORT QCefKitsInit_win(HINSTANCE hInstance, const QCefKitsSettings& settings)
{
    CefMainArgs main_args(hInstance);
    copySettings(settings);
    return initCef(main_args);
}
#endif

bool QCEFKITS_EXPORT QCefKitsInit(int argc, char** argv, const QCefKitsSettings& settings)
{
    CefEnableHighDPISupport();
#ifdef Q_OS_WIN
    (void)argc;
    (void)argv;
    CefMainArgs main_args(::GetModuleHandle(nullptr));
#else
    CefMainArgs main_args(argc, argv);
#endif
    copySettings(settings);
    return initCef(main_args);
}

void QCefKitsShutdown()
{
    //qDebug() << "QCefKitsShutdown 0";
    if (g_timer != nullptr)
    {
        if (g_cefSettings.external_message_pump)
        {
            CefDoMessageLoopWork();
        }
        g_timer->stop();
        g_timer->deleteLater();
    }
    //CefQuitMessageLoop();
    //qApp->processEvents();
    //CefWindowManager::CloseAllWindows();
    //qDebug() << "QCefKitsShutdown 1";
#ifndef OS_LINUX
    CefShutdown();
#elif CHROME_VERSION_MAJOR < 90
    CefShutdown();
#else
    //CefShutdown();    //Shutdown cef90 may hangs in ubuntu
    //cef_shutdown();
#endif
    //qDebug() << "QCefKitsShutdown 2";
}

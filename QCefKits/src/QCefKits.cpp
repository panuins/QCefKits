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
#include "src/CefWebPage.h"
#include "../include/QCefWidget.h"
#include <include/cef_app.h>
#include <QTimer>

CefSettings g_cefSettings;

static QTimer *g_timer = nullptr;

static void copySettings(const QCefKitsSettings& settings)
{
    g_cefSettings.no_sandbox = settings.no_sandbox;
    g_cefSettings.background_color = static_cast<unsigned int>(settings.backgroundColor.rgba());
    g_cefSettings.log_severity = CefHandler::LogLevelToCefLogServerity(settings.logLevel);
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
    if (!settings.userAgentProduction.isEmpty())
    {
        CefString(&g_cefSettings.user_agent_product) = settings.userAgentProduction.toStdWString();
    }
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
    g_timer->start(10);
    QObject::connect(g_timer, &QTimer::timeout,
            g_timer, []()
    {
        if (g_cefSettings.external_message_pump)
        {
            CefDoMessageLoopWork();
        }
        else
        {
            //
        }
    });
    CefRefPtr<CefApp> app(new CefHandler::ClientAppBrowser());
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

bool QCEFKITS_EXPORT QCefKitsInit(int /*argc*/, char** /*argv*/, const QCefKitsSettings& settings)
{
    CefMainArgs main_args(::GetModuleHandle(nullptr));
    copySettings(settings);
    return initCef(main_args);
}

void QCefKitsShutdown()
{
    if (g_timer != nullptr)
    {
        g_timer->stop();
        g_timer->deleteLater();
    }
    CefShutdown();
}

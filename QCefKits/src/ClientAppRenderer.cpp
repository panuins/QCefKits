/****************************************************************************
** CefSubProcess used by QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GPLV2 or later.
****************************************************************************/

#include "ClientAppRenderer.h"
#include "include/base/cef_logging.h"
#include <QDebug>

namespace QCefKits
{

ClientAppRenderer::ClientAppRenderer()
{
}

void ClientAppRenderer::OnWebKitInitialized()
{
    qDebug() << "ClientAppRenderer::OnWebKitInitialized";
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnWebKitInitialized(this);
    }
}

void ClientAppRenderer::OnBrowserCreated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDictionaryValue> extra_info)
{
    qDebug() << "ClientAppRenderer::OnBrowserCreated";
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnBrowserCreated(this, browser, extra_info);
    }
}

void ClientAppRenderer::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
    qDebug() << "ClientAppRenderer::OnBrowserDestroyed";
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnBrowserDestroyed(this, browser);
    }
}

CefRefPtr<CefLoadHandler> ClientAppRenderer::GetLoadHandler()
{
    CefRefPtr<CefLoadHandler> load_handler;
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end() && !load_handler.get(); ++it)
    {
        load_handler = (*it)->GetLoadHandler(this);
    }

    return load_handler;
}

void ClientAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefV8Context> context)
{
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnContextCreated(this, browser, frame, context);
    }
}

void ClientAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefV8Context> context)
{
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnContextReleased(this, browser, frame, context);
    }
}

void ClientAppRenderer::OnUncaughtException(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context,
        CefRefPtr<CefV8Exception> exception,
        CefRefPtr<CefV8StackTrace> stackTrace)
{
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnUncaughtException(this, browser, frame, context, exception,
                                   stackTrace);
    }
}

void ClientAppRenderer::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefDOMNode> node)
{
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnFocusedNodeChanged(this, browser, frame, node);
    }
}

bool ClientAppRenderer::OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message)
{
//    DCHECK_EQ(source_process, PID_BROWSER);

    bool handled = false;

    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end() && !handled; ++it)
    {
        handled = (*it)->OnProcessMessageReceived(this, browser, frame,
                                                  source_process, message);
    }

    return handled;
}
}

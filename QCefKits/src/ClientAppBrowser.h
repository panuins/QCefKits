/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CLIENTAPPBROWSER_H
#define CLIENTAPPBROWSER_H

#include "ClientApp.h"
#include <set>

namespace CefHandler
{

// Client app implementation for the browser process.
class ClientAppBrowser : public ClientApp, public CefBrowserProcessHandler
{
public:
    // Interface for browser delegates. All Delegates must be returned via
    // CreateDelegates. Do not perform work in the Delegate
    // constructor. See CefBrowserProcessHandler for documentation.
    class Delegate : public virtual CefBaseRefCounted
    {
    public:
        virtual void OnBeforeCommandLineProcessing(
                CefRefPtr<ClientAppBrowser> /*app*/,
                CefRefPtr<CefCommandLine> /*command_line*/) {}

        virtual void OnContextInitialized(CefRefPtr<ClientAppBrowser> /*app*/) {}

        virtual void OnBeforeChildProcessLaunch(
                CefRefPtr<ClientAppBrowser> /*app*/,
                CefRefPtr<CefCommandLine> /*command_line*/) {}
    };

    ClientAppBrowser();

private:
    // Creates all of the Delegate objects. Implemented by cefclient in
    // client_app_delegates_browser.cc
    static void CreateDelegates(std::set<CefRefPtr<Delegate> >& delegates);

    // Create the Linux print handler. Implemented by cefclient in
    // client_app_delegates_browser.cc
    static CefRefPtr<CefPrintHandler> CreatePrintHandler();

    // CefApp methods.
    void OnBeforeCommandLineProcessing(
            const CefString& process_type,
            CefRefPtr<CefCommandLine> command_line) OVERRIDE;
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE
    {
        return this;
    }

    // CefBrowserProcessHandler methods.
//    void GetCookieableSchemes(std::vector<CefString>& schemes,
//                              bool& include_defaults) OVERRIDE;
    void OnContextInitialized() OVERRIDE;
    void OnBeforeChildProcessLaunch(
            CefRefPtr<CefCommandLine> command_line) OVERRIDE;
//    CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE
//    {
//        return m_print_handler_;
//    }
    void OnScheduleMessagePumpWork(int64 delay) OVERRIDE;

    // Set of supported Delegates.
    std::set<CefRefPtr<Delegate> > m_delegates_;

//    CefRefPtr<CefPrintHandler> m_print_handler_;

    IMPLEMENT_REFCOUNTING(ClientAppBrowser);
    DISALLOW_COPY_AND_ASSIGN(ClientAppBrowser);
};

}  // namespace CefHandler

#endif // CLIENTAPPBROWSER_H

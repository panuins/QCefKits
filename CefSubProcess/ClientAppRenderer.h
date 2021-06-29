/****************************************************************************
** CefSubProcess used by QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GPLV2 or later.
****************************************************************************/

#ifndef CLIENTAPPRENDERER_H
#define CLIENTAPPRENDERER_H

#include "ClientApp.h"

#include <set>

// Client app implementation for the renderer process.
class ClientAppRenderer : public ClientApp, public CefRenderProcessHandler
{
public:
    // Interface for renderer delegates. All Delegates must be returned via
    // CreateDelegates. Do not perform work in the Delegate
    // constructor. See CefRenderProcessHandler for documentation.
    class Delegate : public virtual CefBaseRefCounted
    {
    public:
        virtual void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> /*app*/) {}

        virtual void OnBrowserCreated(CefRefPtr<ClientAppRenderer> /*app*/,
                                      CefRefPtr<CefBrowser> /*browser*/,
                                      CefRefPtr<CefDictionaryValue> /*extra_info*/) {}

        virtual void OnBrowserDestroyed(CefRefPtr<ClientAppRenderer> /*app*/,
                                        CefRefPtr<CefBrowser> /*browser*/) {}

        virtual CefRefPtr<CefLoadHandler> GetLoadHandler(
                CefRefPtr<ClientAppRenderer> /*app*/)
        {
            return nullptr;
        }

        virtual void OnContextCreated(CefRefPtr<ClientAppRenderer> /*app*/,
                                      CefRefPtr<CefBrowser> /*browser*/,
                                      CefRefPtr<CefFrame> /*frame*/,
                                      CefRefPtr<CefV8Context> /*context*/) {}

        virtual void OnContextReleased(CefRefPtr<ClientAppRenderer> /*app*/,
                                       CefRefPtr<CefBrowser> /*browser*/,
                                       CefRefPtr<CefFrame> /*frame*/,
                                       CefRefPtr<CefV8Context> /*context*/) {}

        virtual void OnUncaughtException(CefRefPtr<ClientAppRenderer> /*app*/,
                                         CefRefPtr<CefBrowser> /*browser*/,
                                         CefRefPtr<CefFrame> /*frame*/,
                                         CefRefPtr<CefV8Context> /*context*/,
                                         CefRefPtr<CefV8Exception> /*exception*/,
                                         CefRefPtr<CefV8StackTrace> /*stackTrace*/) {}

        virtual void OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> /*app*/,
                                          CefRefPtr<CefBrowser> /*browser*/,
                                          CefRefPtr<CefFrame> /*frame*/,
                                          CefRefPtr<CefDOMNode> /*node*/) {}

        // Called when a process message is received. Return true if the message was
        // handled and should not be passed on to other handlers. Delegates
        // should check for unique message names to avoid interfering with each
        // other.
        virtual bool OnProcessMessageReceived(
                CefRefPtr<ClientAppRenderer> /*app*/,
                CefRefPtr<CefBrowser> /*browser*/,
                CefRefPtr<CefFrame> /*frame*/,
                CefProcessId /*source_process*/,
                CefRefPtr<CefProcessMessage> /*message*/)
        {
            return false;
        }
    };

    ClientAppRenderer();

private:
    // Creates all of the Delegate objects. Implemented by cefclient in
    // client_app_delegates_renderer.cc
    static void CreateDelegates(std::set<CefRefPtr<Delegate> >& delegates);

    // CefApp methods.
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE
    {
        return this;
    }

    // CefRenderProcessHandler methods.
    void OnWebKitInitialized() OVERRIDE;
    void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDictionaryValue> extra_info) OVERRIDE;
    void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
    CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) OVERRIDE;
    void OnContextReleased(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefV8Context> context) OVERRIDE;
    void OnUncaughtException(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefV8Context> context,
                             CefRefPtr<CefV8Exception> exception,
                             CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
    void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefDOMNode> node) OVERRIDE;
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
    // Set of supported Delegates.
    std::set<CefRefPtr<Delegate> >  m_delegates_;

    IMPLEMENT_REFCOUNTING(ClientAppRenderer);
    DISALLOW_COPY_AND_ASSIGN(ClientAppRenderer);
};

#endif // CLIENTAPPRENDERER_H

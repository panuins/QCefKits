/****************************************************************************
** CefSubProcess used by QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GPLV2 or later.
****************************************************************************/

#include "ClientRenderDelegate.h"

#include <sstream>
#include <string>

#include "include/cef_crash_util.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"

static const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

void ClientRenderDelegate::OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app)
{
     (void)app;
   // Create the renderer-side router for query handling.
   CefMessageRouterConfig config;
   message_router_ = CefMessageRouterRendererSide::Create(config);
}

void ClientRenderDelegate::OnContextCreated(CefRefPtr<ClientAppRenderer> app,
                                            CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefV8Context> context)
{
    (void)app;
    message_router_->OnContextCreated(browser, frame, context);
}

void ClientRenderDelegate::OnContextReleased(CefRefPtr<ClientAppRenderer> app,
                                             CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefV8Context> context)
{
    (void)app;
    message_router_->OnContextReleased(browser, frame, context);
}

void ClientRenderDelegate::OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> app,
                                                CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                CefRefPtr<CefDOMNode> node)
{
    (void)app;
    (void)browser;
    bool is_editable = (node.get() && node->IsEditable());
    if (is_editable != last_node_is_editable_)
    {
        // Notify the browser of the change in focused element type.
        last_node_is_editable_ = is_editable;
        CefRefPtr<CefProcessMessage> message =
                CefProcessMessage::Create(kFocusedNodeChangedMessage);
        message->GetArgumentList()->SetBool(0, is_editable);
        frame->SendProcessMessage(PID_BROWSER, message);
    }
}

bool ClientRenderDelegate::OnProcessMessageReceived(CefRefPtr<ClientAppRenderer> app,
                                                    CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefProcessId source_process,
                                                    CefRefPtr<CefProcessMessage> message)
{
    (void)app;
    return message_router_->OnProcessMessageReceived(browser, frame,
                                                     source_process, message);
}

void ClientRenderDelegate::CreateDelegates(std::set<CefRefPtr<ClientAppRenderer::Delegate> >& delegates)
{
    delegates.insert(new ClientRenderDelegate);
}

/****************************************************************************
** CefSubProcess used by QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GPLV2 or later.
****************************************************************************/

#ifndef CEF_TESTS_CEFCLIENT_RENDERER_CLIENT_RENDERER_H_
#define CEF_TESTS_CEFCLIENT_RENDERER_CLIENT_RENDERER_H_
#pragma once

#include "ClientAppRenderer.h"
#include <include/cef_base.h>
#include <include/wrapper/cef_message_router.h>

class ClientRenderDelegate : public ClientAppRenderer::Delegate {
 public:
  ClientRenderDelegate() : last_node_is_editable_(false) {}

  void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app) OVERRIDE;

  void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
                        CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) OVERRIDE;

  void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
                         CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefV8Context> context) OVERRIDE;

  void OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> app,
                            CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefDOMNode> node) OVERRIDE;

  bool OnProcessMessageReceived(CefRefPtr<ClientAppRenderer> app,
                                CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) OVERRIDE;

  // Create the renderer delegate. Called from client_app_delegates_renderer.cc.
  static void CreateDelegates(std::set<CefRefPtr<ClientAppRenderer::Delegate> >& delegates);

 private:
  bool last_node_is_editable_;

  // Handles the renderer side of query routing.
  CefRefPtr<CefMessageRouterRendererSide> message_router_;

  DISALLOW_COPY_AND_ASSIGN(ClientRenderDelegate);
  IMPLEMENT_REFCOUNTING(ClientRenderDelegate);
};


#endif  // CEF_TESTS_CEFCLIENT_RENDERER_CLIENT_RENDERER_H_

/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CLIENTREQUESTCONTEXTHANDLER_H
#define CLIENTREQUESTCONTEXTHANDLER_H

#include <include/cef_request_context.h>
#include <include/cef_request_context_handler.h>
#include <include/cef_request_handler.h>
#include <include/cef_web_plugin.h>
#include <include/cef_command_line.h>

namespace QCefKits
{

class ClientRequestContextHandler : public CefRequestContextHandler,
                                    public CefExtensionHandler {
 public:
  ClientRequestContextHandler();

  // CefRequestContextHandler methods:
  bool OnBeforePluginLoad(const CefString& mime_type,
                          const CefString& plugin_url,
                          bool is_main_frame,
                          const CefString& top_origin_url,
                          CefRefPtr<CefWebPluginInfo> plugin_info,
                          PluginPolicy* plugin_policy) override;

  void OnRequestContextInitialized(
      CefRefPtr<CefRequestContext> request_context) override;

  // CefExtensionHandler methods:
  void OnExtensionLoaded(CefRefPtr<CefExtension> extension) override;

  CefRefPtr<CefBrowser> GetActiveBrowser(CefRefPtr<CefExtension> extension,
                                         CefRefPtr<CefBrowser> browser,
                                         bool include_incognito) override;

 private:
  IMPLEMENT_REFCOUNTING(ClientRequestContextHandler);
  DISALLOW_COPY_AND_ASSIGN(ClientRequestContextHandler);
};
}

#endif // CLIENTREQUESTCONTEXTHANDLER_H

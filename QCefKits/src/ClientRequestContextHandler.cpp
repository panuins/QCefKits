/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "ClientRequestContextHandler.h"
#include "CefSwitches.h"
#include <include/wrapper/cef_helpers.h>

namespace CefHandler
{

ClientRequestContextHandler::ClientRequestContextHandler()
{

}

// CefRequestContextHandler methods:
bool ClientRequestContextHandler::OnBeforePluginLoad(const CefString& mime_type,
                                                     const CefString& /*plugin_url*/,
                                                     bool /*is_main_frame*/,
                                                     const CefString& /*top_origin_url*/,
                                                     CefRefPtr<CefWebPluginInfo> /*plugin_info*/,
                                                     PluginPolicy* plugin_policy)
{
  // Always allow the PDF plugin to load.
  if (*plugin_policy != PLUGIN_POLICY_ALLOW &&
      mime_type == "application/pdf") {
    *plugin_policy = PLUGIN_POLICY_ALLOW;
    return true;
  }

  return false;
}

void ClientRequestContextHandler::OnRequestContextInitialized(
    CefRefPtr<CefRequestContext> /*request_context*/)
{
  CEF_REQUIRE_UI_THREAD();

  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  if (command_line->HasSwitch(switches::kLoadExtension)) {
//    if (MainContext::Get()
//            ->GetRootWindowManager()
//            ->request_context_per_browser()) {
//      // The example extension loading implementation requires all browsers to
//      // share the same request context.
//      LOG(ERROR)
//          << "Cannot mix --load-extension and --request-context-per-browser";
//      return;
//    }

    // Load one or more extension paths specified on the command-line and
    // delimited with semicolon.
    const std::string& extension_path =
        command_line->GetSwitchValue(switches::kLoadExtension);
    if (!extension_path.empty()) {
      std::string part;
      std::istringstream f(extension_path);
      while (getline(f, part, ';')) {
//        if (!part.empty())
//          extension_util::LoadExtension(request_context, part, this);
      }
    }
  }
}

// CefExtensionHandler methods:
void ClientRequestContextHandler::OnExtensionLoaded(CefRefPtr<CefExtension> /*extension*/)
{
  CEF_REQUIRE_UI_THREAD();
//  MainContext::Get()->GetRootWindowManager()->AddExtension(extension);
}

CefRefPtr<CefBrowser> ClientRequestContextHandler::GetActiveBrowser(CefRefPtr<CefExtension> /*extension*/,
                                                                    CefRefPtr<CefBrowser> /*browser*/,
                                                                    bool /*include_incognito*/)
{
  CEF_REQUIRE_UI_THREAD();

  return nullptr;
}
}

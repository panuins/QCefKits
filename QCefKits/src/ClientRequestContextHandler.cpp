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

namespace QCefKits
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
            mime_type == "application/pdf")
    {
        *plugin_policy = PLUGIN_POLICY_ALLOW;
        return true;
    }

    return false;
}

void ClientRequestContextHandler::OnRequestContextInitialized(
    CefRefPtr<CefRequestContext> /*request_context*/)
{
    CEF_REQUIRE_UI_THREAD();
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

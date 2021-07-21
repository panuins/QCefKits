/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "ClientBrowserDelegate.h"
#include "CefSwitches.h"

namespace QCefKits {
ClientBrowserDelegate::ClientBrowserDelegate()
{

}

void ClientBrowserDelegate::OnContextInitialized(CefRefPtr<ClientAppBrowser> /*app*/)
{
  const std::string& crl_sets_path =
      CefCommandLine::GetGlobalCommandLine()->GetSwitchValue(
          switches::kCRLSetsPath);
  if (!crl_sets_path.empty()) {
    // Load the CRLSets file from the specified path.
    CefLoadCRLSetsFile(crl_sets_path);
  }
}

void ClientBrowserDelegate::OnBeforeCommandLineProcessing(
    CefRefPtr<ClientAppBrowser> /*app*/,
    CefRefPtr<CefCommandLine> /*command_line*/)
{
  // Append Chromium command line parameters if touch events are enabled
//    if (client::MainContext::Get()->TouchEventsEnabled())
//      command_line->AppendSwitchWithValue("touch-events", "enabled");
}
}

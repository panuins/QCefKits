/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CLIENTBROWSERDELEGATE_H
#define CLIENTBROWSERDELEGATE_H
// Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "ClientAppBrowser.h"
#include <include/cef_command_line.h>
#include <include/cef_crash_util.h>
#include <include/cef_file_util.h>

namespace QCefKits {
class ClientBrowserDelegate : public ClientAppBrowser::Delegate {
 public:
  ClientBrowserDelegate();

  void OnContextInitialized(CefRefPtr<ClientAppBrowser> app) OVERRIDE;
  void OnBeforeCommandLineProcessing(
      CefRefPtr<ClientAppBrowser> app,
      CefRefPtr<CefCommandLine> command_line) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(ClientBrowserDelegate);
  IMPLEMENT_REFCOUNTING(ClientBrowserDelegate);
};

}
#endif // CLIENTBROWSERDELEGATE_H

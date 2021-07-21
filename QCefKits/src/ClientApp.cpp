/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "ClientApp.h"

#include <include/cef_command_line.h>

// These flags must match the Chromium values.
static const char kProcessType[] = "type";
static const char kRendererProcess[] = "renderer";
#if defined(OS_LINUX)
static const char kZygoteProcess[] = "zygote";
#endif

namespace QCefKits
{

ClientApp::ClientApp() {}

// static
ClientApp::ProcessType ClientApp::GetProcessType(
        CefRefPtr<CefCommandLine> command_line)
{
    // The command-line flag won't be specified for the browser process.
    if (!command_line->HasSwitch(kProcessType))
    {
        return BrowserProcess;
    }

    const std::string& process_type = command_line->GetSwitchValue(kProcessType);
    if (process_type == kRendererProcess)
    {
        return RendererProcess;
    }
#if defined(OS_LINUX)
    else if (process_type == kZygoteProcess)
    {
        return ZygoteProcess;
    }
#endif

    return OtherProcess;
}

void ClientApp::OnRegisterCustomSchemes(
        CefRawPtr<CefSchemeRegistrar> registrar)
{
//    RegisterCustomSchemes(registrar, m_cookieable_schemes_);
}

}  // namespace QCefKits

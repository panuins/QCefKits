/****************************************************************************
** CefSubProcess used by QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GPLV2 or later.
****************************************************************************/

#ifndef CLIENTAPP_H
#define CLIENTAPP_H
#include <vector>

#include "include/cef_app.h"

// Base class for customizing process-type-based behavior.
class ClientApp : public CefApp
{
public:
    ClientApp();

    enum ProcessType
    {
        BrowserProcess,
        RendererProcess,
        ZygoteProcess,
        OtherProcess,
    };

    // Determine the process type based on command-line arguments.
    static ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);

protected:
    // Schemes that will be registered with the global cookie manager.
    std::vector<CefString> m_cookieable_schemes_;

private:
    // Registers custom schemes. Implemented by cefclient in
    // client_app_delegates_common.cc
    static void RegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar);

    // CefApp methods.
    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) OVERRIDE;

    DISALLOW_COPY_AND_ASSIGN(ClientApp);
};

#endif // CLIENTAPP_H

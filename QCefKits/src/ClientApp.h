/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CLIENTAPP_H
#define CLIENTAPP_H

#include <include/cef_app.h>
#include <vector>

namespace CefHandler
{

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
    // CefApp methods.
    void OnRegisterCustomSchemes(
            CefRawPtr<CefSchemeRegistrar> registrar) OVERRIDE;

    DISALLOW_COPY_AND_ASSIGN(ClientApp);
};

}  // namespace CefHandler

#endif // CLIENTAPP_H

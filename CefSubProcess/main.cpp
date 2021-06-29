/****************************************************************************
** CefSubProcess used by QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GPLV2 or later.
****************************************************************************/

#include "ClientAppRenderer.h"
#include "ClientAppOther.h"
#include <include/cef_command_line.h>
#include <include/cef_app.h>
#ifdef OS_WIN
#include <Windows.h>
#include <include/cef_sandbox_win.h>
#endif
#include <iostream>

#ifdef OS_WIN
//#if defined(CEF_USE_SANDBOX)
//// The cef_sandbox.lib static library may not link successfully with all VS
//// versions.
//#pragma comment(lib, "cef_sandbox.lib")
//#endif

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE /*hPrevInstance*/,
                      LPTSTR /*lpCmdLine*/,
                      int /*nCmdShow*/)
{
    CefEnableHighDPISupport();
    CefMainArgs main_args(hInstance);
#elif defined(OS_LINUX)
int main(int argc, char **argv)
{
    CefMainArgs main_args(argc, argv);
#elif defined(OS_MAC)
int main(int argc, char **argv)
{
    CefMainArgs main_args(argc, argv);
#endif
    void* sandbox_info = nullptr;

#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
#ifdef OS_WIN
    command_line->InitFromString(::GetCommandLineW());
#elif defined(OS_LINUX)
#elif defined(OS_MAC)
#endif

    CefRefPtr<CefApp> app;
    ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
    if (process_type == ClientApp::RendererProcess)
    {
        app = new ClientAppRenderer();
    }
    else if (process_type == ClientApp::OtherProcess)
    {
        app = new ClientAppOther();
    }
    else
    {
        return -1;
    }

    int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
    return exit_code;
}

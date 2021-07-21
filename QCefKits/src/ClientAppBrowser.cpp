/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "ClientAppBrowser.h"
#include "ClientBrowserDelegate.h"
#include <include/base/cef_logging.h>
#include <include/cef_cookie.h>
#include "CefSwitches.h"

namespace QCefKits
{

ClientAppBrowser::ClientAppBrowser()
{
    CreateDelegates(m_delegates_);
}

void ClientAppBrowser::OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line)
{
    // Pass additional command-line flags to the browser process.
#ifdef OS_LINUX
//    command_line->AppendSwitch("disable-gpu");
//    command_line->AppendSwitch("disable-gpu-compositing");
//    command_line->AppendSwitch("disable-gpu-sandbox");
//    command_line->AppendSwitch("no-zygote");
//    command_line->AppendSwitch("no-sandbox");
//    command_line->AppendSwitch("headless");
#endif
    if (process_type.empty())
    {
        // Pass additional command-line flags when off-screen rendering is enabled.
        if (command_line->HasSwitch(switches::kOffScreenRenderingEnabled) &&
                !command_line->HasSwitch(switches::kSharedTextureEnabled))
        {
            // Use software rendering and compositing (disable GPU) for increased FPS
            // and decreased CPU usage. This will also disable WebGL so remove these
            // switches if you need that capability.
            // See https://bitbucket.org/chromiumembedded/cef/issues/1257 for details.
            if (!command_line->HasSwitch(switches::kEnableGPU))
            {
                command_line->AppendSwitch("disable-gpu");
                command_line->AppendSwitch("disable-gpu-compositing");
            }
        }

        if (command_line->HasSwitch(switches::kUseViews) &&
                !command_line->HasSwitch("top-chrome-md"))
        {
            // Use non-material mode on all platforms by default. Among other things
            // this causes menu buttons to show hover state. See usage of
            // MaterialDesignController::IsModeMaterial() in Chromium code.
            command_line->AppendSwitchWithValue("top-chrome-md", "non-material");
        }

        if (!command_line->HasSwitch(switches::kCachePath) &&
                !command_line->HasSwitch("disable-gpu-shader-disk-cache"))
        {
            // Don't create a "GPUCache" directory when cache-path is unspecified.
            command_line->AppendSwitch("disable-gpu-shader-disk-cache");
        }

#if defined(OS_MAC)
        // Disable the toolchain prompt on macOS.
        command_line->AppendSwitch("use-mock-keychain");
#endif

        std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
        for (; it != m_delegates_.end(); ++it)
        {
            (*it)->OnBeforeCommandLineProcessing(this, command_line);
        }
    }
}

//void ClientAppBrowser::GetCookieableSchemes(std::vector<CefString>& schemes,
//                                            bool& include_defaults)
//{
//    if (!m_cookieable_schemes_.empty())
//    {
//        schemes = m_cookieable_schemes_;
//        include_defaults = true;
//    }
//}

void ClientAppBrowser::OnContextInitialized()
{
//    m_print_handler_ = CreatePrintHandler();

    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnContextInitialized(this);
    }
}

void ClientAppBrowser::OnBeforeChildProcessLaunch(
        CefRefPtr<CefCommandLine> command_line)
{
    std::set<CefRefPtr<Delegate> >::iterator it = m_delegates_.begin();
    for (; it != m_delegates_.end(); ++it)
    {
        (*it)->OnBeforeChildProcessLaunch(this, command_line);
    }
}

void ClientAppBrowser::OnScheduleMessagePumpWork(int64 /*delay*/)
{
    // Only used when `--external-message-pump` is passed via the command-line.
//    MainMessageLoopExternalPump* message_pump =
//            MainMessageLoopExternalPump::Get();
//    if (message_pump)
//        message_pump->OnScheduleMessagePumpWork(delay);
}

// static
//CefRefPtr<CefPrintHandler> ClientAppBrowser::CreatePrintHandler() {
//#if defined(OS_LINUX)
//  return new ClientPrintHandlerGtk();
//#else
//  return nullptr;
//#endif
//}

void ClientAppBrowser::CreateDelegates(std::set<CefRefPtr<Delegate> >& delegates) {
  delegates.insert(new ClientBrowserDelegate);
}

}  // namespace QCefKits

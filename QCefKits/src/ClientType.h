/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CLIENTTYPE_H
#define CLIENTTYPE_H

#include <include/cef_base.h>

#if defined(OS_LINUX)
#include <gtk/gtk.h>

// The Linux client uses GTK instead of the underlying platform type (X11).
typedef GtkWidget *ClientWindowHandle;
#else
typedef CefWindowHandle ClientWindowHandle;
#endif

#if defined(OS_MAC)
typedef void *ClientNativeMacWindow;
#ifdef __OBJC__
#define CAST_CLIENT_NATIVE_MAC_WINDOW_TO_NSWINDOW(native) \
  (__bridge NSWindow*)native
#define CAST_NSWINDOW_TO_CLIENT_NATIVE_MAC_WINDOW(window) (__bridge void*)window
#endif  // __OBJC__
#endif  // defined OS_MAC

#endif // CLIENTTYPE_H

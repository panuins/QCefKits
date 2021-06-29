/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CLIENTUTILS_H
#define CLIENTUTILS_H
#include "../include/QCefKitsGlobal.h"
#include <set>
#include <string>

#include <include/cef_browser.h>
#include <include/cef_request.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>

namespace CefHandler
{

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type);

// Returns the string representation of the specified error code.
std::string GetErrorString(cef_errorcode_t code);

// Show a JS alert message.
void Alert(CefRefPtr<CefBrowser> browser, const std::string& message);

cef_log_severity_t LogLevelToCefLogServerity(enum QCefKits_logLevel level);
QCefKits_logLevel CefLogServerityToLogLevel(cef_log_severity_t l);
}  // namespace CefHandler

#endif // CLIENTUTILS_H

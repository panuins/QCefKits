/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "ClientUtils.h"
#include <include/cef_parser.h>

// Replace all instances of |from| with |to| in |str|.
static std::string StringReplace(const std::string& str,
                                 const std::string& from,
                                 const std::string& to)
{
    std::string result = str;
    std::string::size_type pos = 0;
    std::string::size_type from_len = from.length();
    std::string::size_type to_len = to.length();
    do
    {
        pos = result.find(from, pos);
        if (pos != std::string::npos)
        {
            result.replace(pos, from_len, to);
            pos += to_len;
        }
    } while (pos != std::string::npos);
    return result;
}

namespace QCefKits
{

std::string GetDataURI(const std::string& data, const std::string& mime_type)
{
    return "data:" + mime_type + ";base64," +
            CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
            .ToString();
}

std::string GetErrorString(cef_errorcode_t code)
{
    // Case condition that returns |code| as a string.
#define CASE(code) \
    case code:       \
    return #code

    switch (code)
    {
    CASE(ERR_NONE);
    CASE(ERR_FAILED);
    CASE(ERR_ABORTED);
    CASE(ERR_INVALID_ARGUMENT);
    CASE(ERR_INVALID_HANDLE);
    CASE(ERR_FILE_NOT_FOUND);
    CASE(ERR_TIMED_OUT);
    CASE(ERR_FILE_TOO_BIG);
    CASE(ERR_UNEXPECTED);
    CASE(ERR_ACCESS_DENIED);
    CASE(ERR_NOT_IMPLEMENTED);
    CASE(ERR_CONNECTION_CLOSED);
    CASE(ERR_CONNECTION_RESET);
    CASE(ERR_CONNECTION_REFUSED);
    CASE(ERR_CONNECTION_ABORTED);
    CASE(ERR_CONNECTION_FAILED);
    CASE(ERR_NAME_NOT_RESOLVED);
    CASE(ERR_INTERNET_DISCONNECTED);
    CASE(ERR_SSL_PROTOCOL_ERROR);
    CASE(ERR_ADDRESS_INVALID);
    CASE(ERR_ADDRESS_UNREACHABLE);
    CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
    CASE(ERR_TUNNEL_CONNECTION_FAILED);
    CASE(ERR_NO_SSL_VERSIONS_ENABLED);
    CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
    CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
    CASE(ERR_CERT_COMMON_NAME_INVALID);
    CASE(ERR_CERT_DATE_INVALID);
    CASE(ERR_CERT_AUTHORITY_INVALID);
    CASE(ERR_CERT_CONTAINS_ERRORS);
    CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
    CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
    CASE(ERR_CERT_REVOKED);
    CASE(ERR_CERT_INVALID);
    CASE(ERR_CERT_END);
    CASE(ERR_INVALID_URL);
    CASE(ERR_DISALLOWED_URL_SCHEME);
    CASE(ERR_UNKNOWN_URL_SCHEME);
    CASE(ERR_TOO_MANY_REDIRECTS);
    CASE(ERR_UNSAFE_REDIRECT);
    CASE(ERR_UNSAFE_PORT);
    CASE(ERR_INVALID_RESPONSE);
    CASE(ERR_INVALID_CHUNKED_ENCODING);
    CASE(ERR_METHOD_NOT_SUPPORTED);
    CASE(ERR_UNEXPECTED_PROXY_AUTH);
    CASE(ERR_EMPTY_RESPONSE);
    CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
    CASE(ERR_CACHE_MISS);
    CASE(ERR_INSECURE_RESPONSE);
    default:
        return "UNKNOWN";
    }
}

void Alert(CefRefPtr<CefBrowser> browser, const std::string& message)
{
//    if (browser->GetHost()->GetExtension()) {
        // Alerts originating from extension hosts should instead be displayed in
        // the active browser.
//        browser = MainContext::Get()->GetRootWindowManager()->GetActiveBrowser();
//        if (!browser)
//            return;
//    }

    // Escape special characters in the message.
    std::string msg = StringReplace(message, "\\", "\\\\");
    msg = StringReplace(msg, "'", "\\'");

    // Execute a JavaScript alert().
    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
    frame->ExecuteJavaScript("alert('" + msg + "');", frame->GetURL(), 0);
}

cef_log_severity_t LogLevelToCefLogServerity(enum QCefKits_logLevel level)
{
    switch(level)
    {
    case QCefKits_LogLevel_Default:
        return LOGSEVERITY_DEFAULT;
    case QCefKits_LogLevel_Verbose:
        return LOGSEVERITY_VERBOSE;
    case QCefKits_LogLevel_Debug:
        return LOGSEVERITY_DEBUG;
    case QCefKits_LogLevel_Info:
        return LOGSEVERITY_INFO;
    case QCefKits_LogLevel_Warning:
        return LOGSEVERITY_WARNING;
    case QCefKits_LogLevel_Error:
        return LOGSEVERITY_ERROR;
    case QCefKits_LogLevel_Fatal:
        return LOGSEVERITY_FATAL;
    default:
        return LOGSEVERITY_DISABLE;
    }
}

QCefKits_logLevel CefLogServerityToLogLevel(cef_log_severity_t l)
{
    switch (l)
    {
    case LOGSEVERITY_DEFAULT:
        return QCefKits_LogLevel_Debug;
    case LOGSEVERITY_VERBOSE:
        return QCefKits_LogLevel_Verbose;
//    case LOGSEVERITY_DEBUG:
//        return QCefKits_LogLevel_Debug;
    case LOGSEVERITY_INFO:
        return QCefKits_LogLevel_Info;
    case LOGSEVERITY_WARNING:
        return QCefKits_LogLevel_Warning;
    case LOGSEVERITY_ERROR:
        return QCefKits_LogLevel_Error;
    case LOGSEVERITY_FATAL:
        return QCefKits_LogLevel_Fatal;
    default:
        return QCefKits_LogLevel_Disable;
    }
}

}  // namespace QCefKits

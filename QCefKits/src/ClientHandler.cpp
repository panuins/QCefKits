/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "ClientHandler.h"
#include "FileDialogHandler.h"
#include "JSDialogHandler.h"
#include "CefSwitches.h"
#include <include/base/cef_bind.h>
#include <include/cef_browser.h>
#include <include/cef_frame.h>
#include <include/cef_parser.h>
#include <include/cef_ssl_status.h>
#include <include/cef_x509_certificate.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/cef_command_line.h>
#include <QDebug>

#ifdef OS_LINUX
#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

#include <cstdio>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace QCefKits
{

// Custom menu command Ids.
enum client_menu_ids
{
    CLIENT_ID_SHOW_DEVTOOLS = MENU_ID_USER_FIRST,
    CLIENT_ID_CLOSE_DEVTOOLS,
    CLIENT_ID_INSPECT_ELEMENT,
    CLIENT_ID_SHOW_SSL_INFO,
    CLIENT_ID_CURSOR_CHANGE_DISABLED,
    CLIENT_ID_OFFLINE,
    CLIENT_ID_TESTMENU_SUBMENU,
    CLIENT_ID_TESTMENU_CHECKITEM,
    CLIENT_ID_TESTMENU_RADIOITEM1,
    CLIENT_ID_TESTMENU_RADIOITEM2,
    CLIENT_ID_TESTMENU_RADIOITEM3,
};

// Musr match the value in client_renderer.cc.
static const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

static std::string GetTimeString(const CefTime& value)
{
    if (value.GetTimeT() == 0)
    {
        return "Unspecified";
    }

    static const char* kMonths[] =
    {
        "January", "February", "March",     "April",   "May",      "June",
        "July",    "August",   "September", "October", "November", "December"
    };
    std::string month;
    if (value.month >= 1 && value.month <= 12)
    {
        month = kMonths[value.month - 1];
    }
    else
    {
        month = "Invalid";
    }

    std::stringstream ss;
    ss << month << " " << value.day_of_month << ", " << value.year << " "
       << std::setfill('0') << std::setw(2) << value.hour << ":"
       << std::setfill('0') << std::setw(2) << value.minute << ":"
       << std::setfill('0') << std::setw(2) << value.second;
    return ss.str();
}

static std::string GetBinaryString(CefRefPtr<CefBinaryValue> value)
{
    if (!value.get())
    {
        return "&nbsp;";
    }

    // Retrieve the value.
    const size_t size = value->GetSize();
    std::string src;
    src.resize(size);
    value->GetData(const_cast<char*>(src.data()), size, 0);

    // Encode the value.
    return CefBase64Encode(src.data(), src.size());
}

#define FLAG(flag)                          \
    if (status & flag) {                      \
    result += std::string(#flag) + "<br/>"; \
}

#define VALUE(val, def)       \
    if (val == def) {           \
    return std::string(#def); \
}

static std::string GetCertStatusString(cef_cert_status_t status)
{
    std::string result;

    FLAG(CERT_STATUS_COMMON_NAME_INVALID);
    FLAG(CERT_STATUS_DATE_INVALID);
    FLAG(CERT_STATUS_AUTHORITY_INVALID);
    FLAG(CERT_STATUS_NO_REVOCATION_MECHANISM);
    FLAG(CERT_STATUS_UNABLE_TO_CHECK_REVOCATION);
    FLAG(CERT_STATUS_REVOKED);
    FLAG(CERT_STATUS_INVALID);
    FLAG(CERT_STATUS_WEAK_SIGNATURE_ALGORITHM);
    FLAG(CERT_STATUS_NON_UNIQUE_NAME);
    FLAG(CERT_STATUS_WEAK_KEY);
    FLAG(CERT_STATUS_PINNED_KEY_MISSING);
    FLAG(CERT_STATUS_NAME_CONSTRAINT_VIOLATION);
    FLAG(CERT_STATUS_VALIDITY_TOO_LONG);
    FLAG(CERT_STATUS_IS_EV);
    FLAG(CERT_STATUS_REV_CHECKING_ENABLED);
    FLAG(CERT_STATUS_SHA1_SIGNATURE_PRESENT);
    FLAG(CERT_STATUS_CT_COMPLIANCE_FAILED);

    if (result.empty())
    {
        return "&nbsp;";
    }
    return result;
}

static std::string GetSSLVersionString(cef_ssl_version_t version)
{
    VALUE(version, SSL_CONNECTION_VERSION_UNKNOWN);
    VALUE(version, SSL_CONNECTION_VERSION_SSL2);
    VALUE(version, SSL_CONNECTION_VERSION_SSL3);
    VALUE(version, SSL_CONNECTION_VERSION_TLS1);
    VALUE(version, SSL_CONNECTION_VERSION_TLS1_1);
    VALUE(version, SSL_CONNECTION_VERSION_TLS1_2);
    VALUE(version, SSL_CONNECTION_VERSION_TLS1_3);
    VALUE(version, SSL_CONNECTION_VERSION_QUIC);
    return std::string();
}

static std::string GetContentStatusString(cef_ssl_content_status_t status)
{
    std::string result;

    VALUE(status, SSL_CONTENT_NORMAL_CONTENT);
    FLAG(SSL_CONTENT_DISPLAYED_INSECURE_CONTENT);
    FLAG(SSL_CONTENT_RAN_INSECURE_CONTENT);

    if (result.empty())
        return "&nbsp;";
    return result;
}

// Load a data: URI containing the error message.
static void LoadErrorPage(CefRefPtr<CefFrame> frame,
                          const std::string& failed_url,
                          cef_errorcode_t error_code,
                          const std::string& other_info)
{
    std::stringstream ss;
    ss << "<html><head><title>Page failed to load</title></head>"
          "<body bgcolor=\"white\">"
          "<h3>Page failed to load.</h3>"
          "URL: <a href=\""
       << failed_url << "\">" << failed_url
       << "</a><br/>Error: " << GetErrorString(error_code) << " ("
       << error_code << ")";

    if (!other_info.empty())
        ss << "<br/>" << other_info;

    ss << "</body></html>";
    frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

// Return HTML string with information about a certificate.
static std::string GetCertificateInformation(
        CefRefPtr<CefX509Certificate> cert,
        cef_cert_status_t certstatus)
{
    CefRefPtr<CefX509CertPrincipal> subject = cert->GetSubject();
    CefRefPtr<CefX509CertPrincipal> issuer = cert->GetIssuer();

    // Build a table showing certificate information. Various types of invalid
    // certificates can be tested using https://badssl.com/.
    std::stringstream ss;
    ss << "<h3>X.509 Certificate Information:</h3>"
          "<table border=1><tr><th>Field</th><th>Value</th></tr>";

    if (certstatus != CERT_STATUS_NONE)
    {
        ss << "<tr><td>Status</td><td>" << GetCertStatusString(certstatus)
           << "</td></tr>";
    }

    ss << "<tr><td>Subject</td><td>"
       << (subject.get() ? subject->GetDisplayName().ToString() : "&nbsp;")
       << "</td></tr>"
          "<tr><td>Issuer</td><td>"
       << (issuer.get() ? issuer->GetDisplayName().ToString() : "&nbsp;")
       << "</td></tr>"
          "<tr><td>Serial #*</td><td>"
       << GetBinaryString(cert->GetSerialNumber()) << "</td></tr>"
       << "<tr><td>Valid Start</td><td>" << GetTimeString(cert->GetValidStart())
       << "</td></tr>"
          "<tr><td>Valid Expiry</td><td>"
       << GetTimeString(cert->GetValidExpiry()) << "</td></tr>";

    CefX509Certificate::IssuerChainBinaryList der_chain_list;
    CefX509Certificate::IssuerChainBinaryList pem_chain_list;
    cert->GetDEREncodedIssuerChain(der_chain_list);
    cert->GetPEMEncodedIssuerChain(pem_chain_list);
    DCHECK_EQ(der_chain_list.size(), pem_chain_list.size());

    der_chain_list.insert(der_chain_list.begin(), cert->GetDEREncoded());
    pem_chain_list.insert(pem_chain_list.begin(), cert->GetPEMEncoded());

    for (size_t i = 0U; i < der_chain_list.size(); ++i)
    {
        ss << "<tr><td>DER Encoded*</td>"
              "<td style=\"max-width:800px;overflow:scroll;\">"
           << GetBinaryString(der_chain_list[i])
           << "</td></tr>"
              "<tr><td>PEM Encoded*</td>"
              "<td style=\"max-width:800px;overflow:scroll;\">"
           << GetBinaryString(pem_chain_list[i]) << "</td></tr>";
    }

    ss << "</table> * Displayed value is base64 encoded.";
    return ss.str();
}

class ClientDownloadImageCallback : public CefDownloadImageCallback
{
public:
    explicit ClientDownloadImageCallback(CefRefPtr<CefBrowser> /*browser*/,
                                         CefRefPtr<ClientHandler> client_handler)
        : m_client_handler_(client_handler) {}

    void OnDownloadImageFinished(const CefString& /*image_url*/,
                                 int /*http_status_code*/,
                                 CefRefPtr<CefImage> image) OVERRIDE
    {
        if (image)
        {
            m_client_handler_->NotifyFavicon(m_browser, image);
        }
    }

private:
    CefRefPtr<ClientHandler> m_client_handler_;
    CefRefPtr<CefBrowser> m_browser;

    IMPLEMENT_REFCOUNTING(ClientDownloadImageCallback);
    DISALLOW_COPY_AND_ASSIGN(ClientDownloadImageCallback);
};

ClientHandler::ClientHandler(QSharedPointer<Delegate> delegate,
                             bool is_osr)
    : m_is_osr_(is_osr),
      m_download_favicon_images_(false),
      m_focus_on_editable_field_(false),
      m_initial_navigation_(true)
{
#if defined(OS_LINUX)
    // Provide the GTK-based dialog implementation on Linux.
    m_fileDialogHandler = new FileDialogHandler();
    m_jsDialogHandler = new JSDialogHandler();
#endif
    m_delegate = delegate;

    m_resource_manager_ = new CefResourceManager();
//    test_runner::SetupResourceManager(resource_manager_, &string_resource_map_);

    // Read command line settings.
    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();
    m_mouse_cursor_change_disabled_ =
            command_line->HasSwitch(switches::kMouseCursorChangeDisabled);
    m_offline_ = command_line->HasSwitch(switches::kOffline);
//    qDebug() << "ClientHandler::ClientHandler" << QString::fromStdWString(startup_url);
}

ClientHandler::~ClientHandler()
{
    qDebug() << "ClientHandler::~ClientHandler";
}

bool ClientHandler::OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_message_router_->OnProcessMessageReceived(browser, frame, source_process,
                                                    message))
    {
        return true;
    }

    // Check for messages from the client renderer.
    std::string message_name = message->GetName();
    if (message_name == kFocusedNodeChangedMessage) {
        // A message is sent from ClientRenderDelegate to tell us whether the
        // currently focused DOM node is editable. Use of |focus_on_editable_field_|
        // is redundant with CefKeyEvent.focus_on_editable_field in OnPreKeyEvent
        // but is useful for demonstration purposes.
        m_focus_on_editable_field_ = message->GetArgumentList()->GetBool(0);
        return true;
    }

    return false;
}

void ClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefContextMenuParams> params,
                                        CefRefPtr<CefMenuModel> model) {
    CEF_REQUIRE_UI_THREAD();

    if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) {
        // Add a separator if the menu already has items.
        if (model->GetCount() > 0)
            model->AddSeparator();

        // Add DevTools items to all context menus.
        model->AddItem(CLIENT_ID_SHOW_DEVTOOLS, "&Show DevTools");
        model->AddItem(CLIENT_ID_CLOSE_DEVTOOLS, "Close DevTools");
        model->AddSeparator();
        model->AddItem(CLIENT_ID_INSPECT_ELEMENT, "Inspect Element");

        if (HasSSLInformation(browser)) {
            model->AddSeparator();
            model->AddItem(CLIENT_ID_SHOW_SSL_INFO, "Show SSL information");
        }

        model->AddSeparator();
        model->AddItem(CLIENT_ID_CURSOR_CHANGE_DISABLED, "Cursor change disabled");
        if (m_mouse_cursor_change_disabled_)
        {
            model->SetChecked(CLIENT_ID_CURSOR_CHANGE_DISABLED, true);
        }

        model->AddSeparator();
        model->AddItem(CLIENT_ID_OFFLINE, "Offline mode");
        if (m_offline_)
        {
            model->SetChecked(CLIENT_ID_OFFLINE, true);
        }

        // Test context menu features.
//        BuildTestMenu(model);
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnBeforeContextMenu(browser, frame, params, model);
    }
    else
    {
        qDebug() << "ClientHandler::OnBeforeContextMenu: error: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

bool ClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> /*frame*/,
                                         CefRefPtr<CefContextMenuParams> params,
                                         int command_id,
                                         EventFlags /*event_flags*/)
{
    CEF_REQUIRE_UI_THREAD();

    switch (command_id)
    {
    case CLIENT_ID_SHOW_DEVTOOLS:
        ShowDevTools(browser, CefPoint());
        return true;
    case CLIENT_ID_CLOSE_DEVTOOLS:
        CloseDevTools(browser);
        return true;
    case CLIENT_ID_INSPECT_ELEMENT:
        ShowDevTools(browser, CefPoint(params->GetXCoord(), params->GetYCoord()));
        return true;
    case CLIENT_ID_SHOW_SSL_INFO:
        ShowSSLInformation(browser);
        return true;
    case CLIENT_ID_CURSOR_CHANGE_DISABLED:
        m_mouse_cursor_change_disabled_ = !m_mouse_cursor_change_disabled_;
        return true;
    case CLIENT_ID_OFFLINE:
        m_offline_ = !m_offline_;
        SetOfflineState(browser, m_offline_);
        return true;
    default:  // Allow default handling, if any.
        return true;
    }
}

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    const CefString& url)
{
    CEF_REQUIRE_UI_THREAD();

    // Only update the address for the main (top-level) frame.
    if (frame->IsMain())
    {
        NotifyAddress(browser, url);
    }
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();

    NotifyTitle(browser, title);
}

void ClientHandler::OnFaviconURLChange(
        CefRefPtr<CefBrowser> browser,
        const std::vector<CefString>& icon_urls)
{
    CEF_REQUIRE_UI_THREAD();

    if (!icon_urls.empty() && m_download_favicon_images_)
    {
        browser->GetHost()->DownloadImage(icon_urls[0], true, 16, false,
                new ClientDownloadImageCallback(browser, this));
    }
}

void ClientHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
                                           bool fullscreen) {
    CEF_REQUIRE_UI_THREAD();

    NotifyFullscreen(browser, fullscreen);
}

bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                     cef_log_severity_t level,
                                     const CefString& message,
                                     const CefString& source,
                                     int line)
{
    CEF_REQUIRE_UI_THREAD();

//    qDebug() << "ClientHandler::OnConsoleMessage"
//             << level
//             << "Message: " << QString::fromStdString(message.ToString())
//             << "Source: " << QString::fromStdString(source.ToString())
//             << "Line: " << line;

    if (!m_delegate.isNull())
    {
        return m_delegate->OnConsoleMessage(browser, level, message, source, line);
    }
    else
    {
        qDebug() << "ClientHandler::OnConsoleMessage: error: error: m_delegate is null"
                 << browser->GetIdentifier();
    }

    return false;
}

bool ClientHandler::OnAutoResize(CefRefPtr<CefBrowser> browser,
                                 const CefSize& new_size)
{
    CEF_REQUIRE_UI_THREAD();

    NotifyAutoResize(browser, new_size);
    return true;
}

bool ClientHandler::OnCursorChange(CefRefPtr<CefBrowser> /*browser*/,
                                   CefCursorHandle /*cursor*/,
                                   cef_cursor_type_t /*type*/,
                                   const CefCursorInfo& /*custom_cursor_info*/)
{
    CEF_REQUIRE_UI_THREAD();

    // Return true to disable default handling of cursor changes.
    return m_mouse_cursor_change_disabled_;
}

void ClientHandler::OnBeforeDownload(
        CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefDownloadItem> /*download_item*/,
        const CefString& /*suggested_name*/,
        CefRefPtr<CefBeforeDownloadCallback> /*callback*/)
{
    CEF_REQUIRE_UI_THREAD();

    // Continue the download and show the "Save As" dialog.
//    callback->Continue(MainContext::Get()->GetDownloadPath(suggested_name), true);
}

void ClientHandler::OnDownloadUpdated(
        CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefDownloadItem> /*download_item*/,
        CefRefPtr<CefDownloadItemCallback> /*callback*/)
{
    CEF_REQUIRE_UI_THREAD();

//    if (download_item->IsComplete()) {
//        test_runner::Alert(browser, "File \"" +
//                           download_item->GetFullPath().ToString() +
//                           "\" downloaded successfully.");
//    }
}

bool ClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDragData> dragData,
                                CefDragHandler::DragOperationsMask mask)
{
    CEF_REQUIRE_UI_THREAD();

//    // Forbid dragging of URLs and files.
//    if ((mask & DRAG_OPERATION_LINK) && !dragData->IsFragment())
//    {
//        Alert(browser, "cefclient blocks dragging of URLs and files");
//        return true;
//    }

    return false;
}

void ClientHandler::OnDraggableRegionsChanged(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> /*frame*/,
        const std::vector<CefDraggableRegion>& regions)
{
    CEF_REQUIRE_UI_THREAD();

    NotifyDraggableRegions(browser, regions);
}

void ClientHandler::OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next)
{
    CEF_REQUIRE_UI_THREAD();

//    qDebug() << "ClientHandler::OnTakeFocus";
    NotifyTakeFocus(browser, next);
}

void ClientHandler::OnGotFocus(CefRefPtr<CefBrowser> browser)
{
//    qDebug() << "ClientHandler::OnGotFocus";
    if (!m_delegate)
    {
        return;
    }
#ifdef Q_OS_LINUX
    Display *cef_display = cef_get_xdisplay();
    WId window_handle = browser->GetHost()->GetWindowHandle();
    Window cef_focus_window = 0;
    int revert_to = 0;

    XGetInputFocus(cef_display, &cef_focus_window, &revert_to);

    Window root_window = 0;
    Window parent_window = cef_focus_window;

    // cef中当鼠标enter到cef窗口区域时就会触发此事件
    // 所以此处需要判断当前的焦点窗口是不是对应的cef窗口，或它的子窗口
    forever
    {
        if (parent_window == window_handle)
        {
            m_delegate->OnGotFocus(browser);
            break;
        }

        // 已经是顶层窗口
        if (parent_window == root_window)
        {
            break;
        }

        Window *child = nullptr;
        uint child_count = 0;
        int s = XQueryTree(cef_display, parent_window, &root_window, &parent_window, &child, &child_count);

        if (!child)
        {
            XFree(child);
        }

        if (s == 0)
        { // error
            break;
        }
    }
#endif
}

bool ClientHandler::OnSetFocus(CefRefPtr<CefBrowser> /*browser*/,
                               FocusSource /*source*/)
{
    CEF_REQUIRE_UI_THREAD();
    qDebug() << "ClientHandler::OnSetFocus";

    if (m_initial_navigation_)
    {
        CefRefPtr<CefCommandLine> command_line =
                CefCommandLine::GetGlobalCommandLine();
        if (command_line->HasSwitch(switches::kNoActivate))
        {
            // Don't give focus to the browser on creation.
            return true;
        }
    }

    return true;
}

bool ClientHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                  const CefKeyEvent& event,
                                  CefEventHandle /*os_event*/,
                                  bool* /*is_keyboard_shortcut*/)
{
    CEF_REQUIRE_UI_THREAD();

//    if (!event.focus_on_editable_field && event.windows_key_code == 0x20)
//    {
//        // Special handling for the space character when an input element does not
//        // have focus. Handling the event in OnPreKeyEvent() keeps the event from
//        // being processed in the renderer. If we instead handled the event in the
//        // OnKeyEvent() method the space key would cause the window to scroll in
//        // addition to showing the alert box.
//        if (event.type == KEYEVENT_RAWKEYDOWN)
//            Alert(browser, "You pressed the space bar!");
//        return true;
//    }

    return false;
}

bool ClientHandler::OnBeforePopup(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> /*frame*/,
        const CefString& /*target_url*/,
        const CefString& /*target_frame_name*/,
        CefLifeSpanHandler::WindowOpenDisposition target_disposition,
        bool /*user_gesture*/,
        const CefPopupFeatures& popupFeatures,
        CefWindowInfo& windowInfo,
        CefRefPtr<CefClient>& client,
        CefBrowserSettings& settings,
        CefRefPtr<CefDictionaryValue>& /*extra_info*/,
        bool* /*no_javascript_access*/)
{
    CEF_REQUIRE_UI_THREAD();

    // Return true to cancel the popup window.
    return !CreatePopupWindow(browser, false, target_disposition,
                              popupFeatures, windowInfo, client,
                              settings);
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (!m_message_router_)
    {
        // Create the browser-side router for query handling.
        CefMessageRouterConfig config;
        m_message_router_ = CefMessageRouterBrowserSide::Create(config);

        // Register handlers with the router.
//        test_runner::CreateMessageHandlers(message_handler_set_);
        std::set<CefMessageRouterBrowserSide::Handler*>::const_iterator it = m_message_handler_set_.begin();
        for (; it != m_message_handler_set_.end(); ++it)
        {
            m_message_router_->AddHandler(*(it), false);
        }
    }

    // Set offline mode if requested via the command-line flag.
    if (m_offline_)
    {
        SetOfflineState(browser, true);
    }

    if (browser->GetHost()->GetExtension())
    {
        // Browsers hosting extension apps should auto-resize.
        browser->GetHost()->SetAutoResizeEnabled(true, CefSize(20, 20),
                                                 CefSize(1000, 1000));

//        CefRefPtr<CefExtension> extension = browser->GetHost()->GetExtension();
//        if (extension_util::IsInternalExtension(extension->GetPath())) {
//            // Register the internal handler for extension resources.
//            extension_util::AddInternalExtensionToResourceManager(extension,
//                                                                  resource_manager_);
//        }
    }

    NotifyBrowserCreated(browser);
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (!m_delegate.isNull())
    {
        return m_delegate->DoClose(browser);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyBrowserClosing: warning: m_delegate is null"
                 << browser->GetIdentifier();
    }
    return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    qDebug() << "ClientHandler::OnBeforeClose";
    CEF_REQUIRE_UI_THREAD();

    // Remove and delete message router handlers.
    std::set<CefMessageRouterBrowserSide::Handler*>::const_iterator it = m_message_handler_set_.begin();
    for (; it != m_message_handler_set_.end(); ++it)
    {
        m_message_router_->RemoveHandler(*(it));
        delete *(it);
    }
    m_message_handler_set_.clear();
    m_message_router_ = nullptr;

    NotifyBrowserClosed(browser);
}

void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                TransitionType transition_type)
{
//    qDebug() << "ClientHandler::OnLoadStart" << transition_type;
    if (!m_delegate.isNull())
    {
        m_delegate->OnLoadStart(browser, frame, transition_type);
    }
    else
    {
        qDebug() << "ClientHandler::OnLoadStart: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              int httpStatusCode)
{
//    qDebug() << "ClientHandler::OnLoadEnd" << httpStatusCode;
    if (!m_delegate.isNull())
    {
        m_delegate->OnLoadEnd(browser, frame, httpStatusCode);
    }
    else
    {
        qDebug() << "ClientHandler::OnLoadEnd: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                         bool isLoading,
                                         bool canGoBack,
                                         bool canGoForward)
{
    CEF_REQUIRE_UI_THREAD();

    if (!isLoading && m_initial_navigation_)
    {
        m_initial_navigation_ = false;
    }

    NotifyLoadingState(browser, isLoading, canGoBack, canGoForward);
}

void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> /*browser*/,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();
//    qDebug() << "ClientHandler::OnLoadError"
//             << errorCode
//             << QString::fromStdWString(errorText.ToWString());

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
    {
        return;
    }

    // Don't display an error for external protocols that we allow the OS to
    // handle. See OnProtocolExecution().
    if (errorCode == ERR_UNKNOWN_URL_SCHEME)
    {
        std::string urlStr = frame->GetURL();
        if (urlStr.find("spotify:") == 0)
        {
            return;
        }
    }

    // Load the error page.
    LoadErrorPage(frame, failedUrl, errorCode, errorText);
}

bool ClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> /*request*/,
                                   bool /*user_gesture*/,
                                   bool /*is_redirect*/)
{
    CEF_REQUIRE_UI_THREAD();
//    qDebug() << "ClientHandler::OnBeforeBrowse";

    m_message_router_->OnBeforeBrowse(browser, frame);
    return false;
}

//TODO
bool ClientHandler::OnOpenURLFromTab(
        CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefFrame> /*frame*/,
        const CefString& /*target_url*/,
        CefRequestHandler::WindowOpenDisposition target_disposition,
        bool /*user_gesture*/)
{
    if (target_disposition == WOD_NEW_BACKGROUND_TAB ||
            target_disposition == WOD_NEW_FOREGROUND_TAB)
    {
        // Handle middle-click and ctrl + left-click by opening the URL in a new
        // browser window.
//        RootWindowConfig config;
//        config.with_controls = true;
//        config.with_osr = is_osr();
//        config.url = target_url;
//        MainContext::Get()->GetRootWindowManager()->CreateRootWindow(config);
        return true;
    }

    // Open the URL in the current browser window.
    return false;
}

CefRefPtr<CefResourceRequestHandler> ClientHandler::GetResourceRequestHandler(
        CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefFrame> /*frame*/,
        CefRefPtr<CefRequest> /*request*/,
        bool /*is_navigation*/,
        bool /*is_download*/,
        const CefString& /*request_initiator*/,
        bool& /*disable_default_handling*/)
{
    CEF_REQUIRE_IO_THREAD();
    return this;
}

bool ClientHandler::GetAuthCredentials(CefRefPtr<CefBrowser> /*browser*/,
                                       const CefString& /*origin_url*/,
                                       bool isProxy,
                                       const CefString& host,
                                       int /*port*/,
                                       const CefString& /*realm*/,
                                       const CefString& /*scheme*/,
                                       CefRefPtr<CefAuthCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();

    // Used for testing authentication with a proxy server.
    // For example, CCProxy on Windows.
    if (isProxy)
    {
        callback->Continue("guest", "guest");
        return true;
    }

    return false;
}

bool ClientHandler::OnQuotaRequest(CefRefPtr<CefBrowser> /*browser*/,
                                   const CefString& /*origin_url*/,
                                   int64 new_size,
                                   CefRefPtr<CefRequestCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();

    static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

    // Grant the quota request if the size is reasonable.
    callback->Continue(new_size <= max_size);
    return true;
}

bool ClientHandler::OnCertificateError(CefRefPtr<CefBrowser> browser,
                                       ErrorCode cert_error,
                                       const CefString& request_url,
                                       CefRefPtr<CefSSLInfo> ssl_info,
                                       CefRefPtr<CefRequestCallback> callback)
{
    CEF_REQUIRE_UI_THREAD();

//    if (cert_error == ERR_CERT_AUTHORITY_INVALID &&
//            request_url.ToString().find("https://www.magpcss.org/") == 0U) {
//        // Allow the CEF Forum to load. It has a self-signed certificate.
//        callback->Continue(true);
//        return true;
//    }

    CefRefPtr<CefX509Certificate> cert = ssl_info->GetX509Certificate();
    if (cert.get())
    {
        // Load the error page.
        LoadErrorPage(browser->GetMainFrame(), request_url, cert_error,
                      GetCertificateInformation(cert, ssl_info->GetCertStatus()));
    }

    return false;  // Cancel the request.
}

bool ClientHandler::OnSelectClientCertificate(
        CefRefPtr<CefBrowser> /*browser*/,
        bool /*isProxy*/,
        const CefString& /*host*/,
        int /*port*/,
        const X509CertificateList& certificates,
        CefRefPtr<CefSelectClientCertificateCallback> callback)
{
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();
    if (!command_line->HasSwitch(switches::kSslClientCertificate))
    {
        return false;
    }

    const std::string& cert_name =
            command_line->GetSwitchValue(switches::kSslClientCertificate);

    if (cert_name.empty())
    {
        callback->Select(nullptr);
        return true;
    }

    std::vector<CefRefPtr<CefX509Certificate>>::const_iterator it =
            certificates.begin();
    for (; it != certificates.end(); ++it)
    {
        CefString subject((*it)->GetSubject()->GetDisplayName());
        if (subject == cert_name)
        {
            callback->Select(*it);
            return true;
        }
    }

    return true;
}

void ClientHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                              TerminationStatus /*status*/)
{
    CEF_REQUIRE_UI_THREAD();

    m_message_router_->OnRenderProcessTerminated(browser);
}

void ClientHandler::OnDocumentAvailableInMainFrame(
        CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Restore offline mode after main frame navigation. Otherwise, offline state
    // (e.g. `navigator.onLine`) might be wrong in the renderer process.
    if (m_offline_)
    {
        SetOfflineState(browser, true);
    }
}

cef_return_value_t ClientHandler::OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefRequestCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();

//    qDebug() << "ClientHandler::OnBeforeResourceLoad";
//    callback->Continue(true);
    return m_resource_manager_->OnBeforeResourceLoad(browser, frame, request,
                                                   callback);
//    return RV_CONTINUE;
}

CefRefPtr<CefResourceHandler> ClientHandler::GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();

//    qDebug() << "ClientHandler::GetResourceHandler";
    return m_resource_manager_->GetResourceHandler(browser, frame, request);
}

CefRefPtr<CefResponseFilter> ClientHandler::GetResourceResponseFilter(
        CefRefPtr<CefBrowser> /*browser*/,
        CefRefPtr<CefFrame> /*frame*/,
        CefRefPtr<CefRequest> /*request*/,
        CefRefPtr<CefResponse> /*response*/)
{
    CEF_REQUIRE_IO_THREAD();

//    qDebug() << "ClientHandler::GetResourceResponseFilter";
    return nullptr;/*GetResourceResponseFilter(browser, frame, request,
                                                  response);*/
}

void ClientHandler::OnProtocolExecution(CefRefPtr<CefBrowser> /*browser*/,
                                        CefRefPtr<CefFrame> /*frame*/,
                                        CefRefPtr<CefRequest> request,
                                        bool& allow_os_execution)
{
    CEF_REQUIRE_IO_THREAD();

//    std::string urlStr = request->GetURL();
}

void ClientHandler::ShowDevTools(CefRefPtr<CefBrowser> browser,
                                 const CefPoint& inspect_element_at)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the UI thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::ShowDevTools, this, browser,
                                       inspect_element_at));
        return;
    }

    CefWindowInfo windowInfo;
    CefRefPtr<CefClient> client;
    CefBrowserSettings settings;

//    MainContext::Get()->PopulateBrowserSettings(&settings);

    CefRefPtr<CefBrowserHost> host = browser->GetHost();

    // Test if the DevTools browser already exists.
    bool has_devtools = host->HasDevTools();
    if (!has_devtools)
    {
        // Create a new RootWindow for the DevTools browser that will be created
        // by ShowDevTools().
        has_devtools = CreatePopupWindow(browser, true, WOD_NEW_POPUP, CefPopupFeatures(),
                                         windowInfo, client, settings);
    }

    if (has_devtools)
    {
        // Create the DevTools browser if it doesn't already exist.
        // Otherwise, focus the existing DevTools browser and inspect the element
        // at |inspect_element_at| if non-empty.
        host->ShowDevTools(windowInfo, client, settings, inspect_element_at);
    }
}

void ClientHandler::CloseDevTools(CefRefPtr<CefBrowser> browser)
{
    browser->GetHost()->CloseDevTools();
}

bool ClientHandler::HasSSLInformation(CefRefPtr<CefBrowser> browser)
{
    CefRefPtr<CefNavigationEntry> nav =
            browser->GetHost()->GetVisibleNavigationEntry();

    return (nav && nav->GetSSLStatus() &&
            nav->GetSSLStatus()->IsSecureConnection());
}

void ClientHandler::ShowSSLInformation(CefRefPtr<CefBrowser> browser)
{
    std::stringstream ss;
    CefRefPtr<CefNavigationEntry> nav =
            browser->GetHost()->GetVisibleNavigationEntry();
    if (!nav)
        return;

    CefRefPtr<CefSSLStatus> ssl = nav->GetSSLStatus();
    if (!ssl)
        return;

    ss << "<html><head><title>SSL Information</title></head>"
          "<body bgcolor=\"white\">"
          "<h3>SSL Connection</h3>"
       << "<table border=1><tr><th>Field</th><th>Value</th></tr>";

    CefURLParts urlparts;
    if (CefParseURL(nav->GetURL(), urlparts))
    {
        CefString port(&urlparts.port);
        ss << "<tr><td>Server</td><td>" << CefString(&urlparts.host).ToString();
        if (!port.empty())
            ss << ":" << port.ToString();
        ss << "</td></tr>";
    }

    ss << "<tr><td>SSL Version</td><td>"
       << GetSSLVersionString(ssl->GetSSLVersion()) << "</td></tr>";
    ss << "<tr><td>Content Status</td><td>"
       << GetContentStatusString(ssl->GetContentStatus()) << "</td></tr>";

    ss << "</table>";

    CefRefPtr<CefX509Certificate> cert = ssl->GetX509Certificate();
    if (cert.get())
        ss << GetCertificateInformation(cert, ssl->GetCertStatus());

    ss << "</body></html>";

//    RootWindowConfig config;
//    config.with_controls = false;
//    config.with_osr = is_osr();
//    config.url = test_runner::GetDataURI(ss.str(), "text/html");
//    MainContext::Get()->GetRootWindowManager()->CreateRootWindow(config);
}

void ClientHandler::SetStringResource(const std::string& page,
                                      const std::string& data)
{
    if (!CefCurrentlyOn(TID_IO))
    {
        CefPostTask(TID_IO, base::Bind(&ClientHandler::SetStringResource, this,
                                       page, data));
        return;
    }

    m_string_resource_map_[page] = data;
}

//TODO onbeforepopup
bool ClientHandler::CreatePopupWindow(CefRefPtr<CefBrowser> browser,
                                      bool is_devtools,
                                      CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                                      const CefPopupFeatures& popupFeatures,
                                      CefWindowInfo& windowInfo,
                                      CefRefPtr<CefClient>& client,
                                      CefBrowserSettings& settings)
{
    CEF_REQUIRE_UI_THREAD();

    QSharedPointer<Delegate> newDelegate;
    if (!m_delegate.isNull())
    {
        newDelegate = m_delegate->CreatePopupWindow(browser, is_devtools,
                                                    target_disposition,
                                                    popupFeatures,
                                                    windowInfo,
                                                    settings);
    }
    else
    {
        qDebug() << "ClientHandler::CreatePopupWindow: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
    if (newDelegate)
    {
        client = new ClientHandler(newDelegate, false);
    }
    // The popup browser will be parented to a new native window.
    // Don't show URL bar and navigation buttons on DevTools windows.
//    MainContext::Get()->GetRootWindowManager()->CreateRootWindowAsPopup(
//                !is_devtools, is_osr(), popupFeatures, windowInfo, client, settings);

    return true;
}

void ClientHandler::NotifyBrowserCreated(CefRefPtr<CefBrowser> browser)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyBrowserCreated, this, browser));
        return;
    }

//    qDebug() << "ClientHandler::NotifyBrowserCreated";
    if (!m_delegate.isNull())
    {
        m_delegate->OnBrowserCreated(browser);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyBrowserCreated: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyBrowserClosing(CefRefPtr<CefBrowser> browser)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyBrowserClosing, this, browser));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->DoClose(browser);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyBrowserClosing: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyBrowserClosed(CefRefPtr<CefBrowser> browser)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyBrowserClosed, this, browser));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnBrowserClosed(browser);
        m_delegate.clear();
    }
    else
    {
        qDebug() << "ClientHandler::NotifyBrowserClosed: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyAddress(CefRefPtr<CefBrowser> browser, const CefString& url)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyAddress, this, browser, url));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnSetAddress(browser, url);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyAddress: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyTitle(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyTitle, this, browser, title));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnSetTitle(browser, title);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyTitle: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyFavicon(CefRefPtr<CefBrowser> browser, CefRefPtr<CefImage> image)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyFavicon, this, browser, image));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnSetFavicon(browser, image);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyFavicon: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyFullscreen, this, browser, fullscreen));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnSetFullscreen(browser, fullscreen);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyFullscreen: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyAutoResize, this, browser, new_size));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnAutoResize(browser, new_size);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyAutoResize: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyLoadingState(CefRefPtr<CefBrowser> browser,
                                       bool isLoading,
                                       bool canGoBack,
                                       bool canGoForward)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyLoadingState, this,
                                       browser, isLoading, canGoBack, canGoForward));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnSetLoadingState(browser, isLoading, canGoBack, canGoForward);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyLoadingState: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyDraggableRegions(
        CefRefPtr<CefBrowser> browser,
        const std::vector<CefDraggableRegion>& regions)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyDraggableRegions, this, browser, regions));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnSetDraggableRegions(browser, regions);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyDraggableRegions: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::NotifyTakeFocus(CefRefPtr<CefBrowser> browser, bool next)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI, base::Bind(&ClientHandler::NotifyTakeFocus, this, browser, next));
        return;
    }

    if (!m_delegate.isNull())
    {
        m_delegate->OnTakeFocus(browser, next);
    }
    else
    {
        qDebug() << "ClientHandler::NotifyTakeFocus: error: m_delegate is null"
                 << browser->GetIdentifier();
    }
}

void ClientHandler::SetOfflineState(CefRefPtr<CefBrowser> browser,
                                    bool offline)
{
    // See DevTools protocol docs for message format specification.
    CefRefPtr<CefDictionaryValue> params = CefDictionaryValue::Create();
    params->SetBool("offline", offline);
    params->SetDouble("latency", 0);
    params->SetDouble("downloadThroughput", 0);
    params->SetDouble("uploadThroughput", 0);
    browser->GetHost()->ExecuteDevToolsMethod(
                /*message_id=*/0, "Network.emulateNetworkConditions", params);
}

}  // namespace QCefKits

/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H


#include "ClientType.h"
#include "ClientUtils.h"
#include <include/cef_version.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_helpers.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#include <QMap>
#include <QSharedPointer>

#include <set>
#include <string>

namespace QCefKits
{

class ClientDownloadImageCallback;

// Client handler abstract base class. Provides common functionality shared by
// all concrete client handler implementations.
class ClientHandler : public CefClient,
        public CefContextMenuHandler,
        public CefDisplayHandler,
        public CefDownloadHandler,
        public CefDragHandler,
        public CefFocusHandler,
        public CefKeyboardHandler,
        public CefLifeSpanHandler,
        public CefLoadHandler,
        public CefRequestHandler,
        public CefResourceRequestHandler
{
public:
    // Implement this interface to receive notification of ClientHandler
    // events. The methods of this class will be called on the main thread unless
    // otherwise indicated.
    class Delegate {
    public:
        // Called when the browser is created.
        virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) = 0;

        // Called when the browser has been closed.
        virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) = 0;
        virtual bool DoClose(CefRefPtr<CefBrowser> browser) = 0;
        virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                      cef_log_severity_t level,
                                      const CefString& message,
                                      const CefString& source,
                                      int line) = 0;
        // Set the window URL address.
        virtual void OnSetAddress(CefRefPtr<CefBrowser> browser, const std::string& url) = 0;

        // Set the window title.
        virtual void OnSetTitle(CefRefPtr<CefBrowser> browser, const std::string& title) = 0;

        // Set the Favicon image.
        virtual void OnSetFavicon(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefImage> /*image*/) {}

        // Set fullscreen mode.
        virtual void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) = 0;

        // Auto-resize contents.
        virtual void OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size) = 0;

        // Set the loading state.
        virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 TransitionType transition_type) = 0;
        virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               int httpStatusCode) = 0;
        virtual void OnSetLoadingState(CefRefPtr<CefBrowser> browser,
                                       bool isLoading,
                                       bool canGoBack,
                                       bool canGoForward) = 0;
        virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 ErrorCode errorCode,
                                 const CefString& errorText,
                                 const CefString& failedUrl) = 0;

        // Set the draggable regions.
        virtual void OnSetDraggableRegions(
                CefRefPtr<CefBrowser> browser,
                const std::vector<CefDraggableRegion>& regions) = 0;

        // Set focus to the next/previous control.
        virtual void OnGotFocus(CefRefPtr<CefBrowser> /*browser*/) {}
        virtual bool OnSetFocus(CefRefPtr<CefBrowser> /*browser*/, FocusSource /*source*/) {return false;}
        virtual void OnTakeFocus(CefRefPtr<CefBrowser> /*browser*/, bool /*next*/) {}

        // Called on the UI thread before a context menu is displayed.
        virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> /*browser*/,
                                         CefRefPtr<CefFrame> /*frame*/,
                                         CefRefPtr<CefContextMenuParams> /*params*/,
                                         CefRefPtr<CefMenuModel> /*model*/) {}
        virtual QSharedPointer<Delegate> CreatePopupWindow(CefRefPtr<CefBrowser> browser,
                                                           bool is_devtools,
                                                           CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                                                           const CefPopupFeatures& popupFeatures,
                                                           CefWindowInfo& windowInfo,
                                                           CefBrowserSettings& settings) = 0;
    protected:
        virtual ~Delegate() {}
    };

//    typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

    // Constructor may be called on any thread.
    // |delegate| must outlive this object or DetachDelegate() must be called.
    ClientHandler(QSharedPointer<Delegate> delegate,
                  bool is_osr);
    ~ClientHandler() override;

    // CefClient methods
    CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override {
        return this;
    }
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() override { return this; }
    CefRefPtr<CefDragHandler> GetDragHandler() override { return this; }
    CefRefPtr<CefFocusHandler> GetFocusHandler() override { return this; }
    CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
    CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

#if defined(OS_LINUX)
    CefRefPtr<CefDialogHandler> GetDialogHandler() override {
        return m_fileDialogHandler;
    }
    CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override {
        return m_jsDialogHandler;
    }
#endif

    // CefContextMenuHandler methods
    void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefContextMenuParams> params,
                             CefRefPtr<CefMenuModel> model) override;
    bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefContextMenuParams> params,
                              int command_id,
                              EventFlags event_flags) override;

    // CefDisplayHandler methods
    void OnAddressChange(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         const CefString& url) override;
    void OnTitleChange(CefRefPtr<CefBrowser> browser,
                       const CefString& title) override;
    void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                            const std::vector<CefString>& icon_urls) override;
    void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
                                bool fullscreen) override;
    bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                          cef_log_severity_t level,
                          const CefString& message,
                          const CefString& source,
                          int line) override;
    bool OnAutoResize(CefRefPtr<CefBrowser> browser,
                      const CefSize& new_size) override;
    bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                        CefCursorHandle cursor,
                        cef_cursor_type_t type,
                        const CefCursorInfo& custom_cursor_info) override;

    // CefDownloadHandler methods
    void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDownloadItem> download_item,
                          const CefString& suggested_name,
                          CefRefPtr<CefBeforeDownloadCallback> callback) override;
    void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDownloadItem> download_item,
                           CefRefPtr<CefDownloadItemCallback> callback) override;

    // CefDragHandler methods
    bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefDragData> dragData,
                     CefDragHandler::DragOperationsMask mask) override;
    void OnDraggableRegionsChanged(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            const std::vector<CefDraggableRegion>& regions) override;

    // CefFocusHandler methods
    void OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) override;
    void OnGotFocus(CefRefPtr<CefBrowser> browser) override;
    bool OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source) override;

    // CefKeyboardHandler methods
    bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                       const CefKeyEvent& event,
                       CefEventHandle os_event,
                       bool* is_keyboard_shortcut) override;

    // CefLifeSpanHandler methods
    bool OnBeforePopup(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            const CefString& target_url,
            const CefString& target_frame_name,
            CefLifeSpanHandler::WindowOpenDisposition target_disposition,
            bool user_gesture,
            const CefPopupFeatures& popupFeatures,
            CefWindowInfo& windowInfo,
            CefRefPtr<CefClient>& client,
            CefBrowserSettings& settings,
            CefRefPtr<CefDictionaryValue>& extra_info,
            bool* no_javascript_access) override;
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    bool DoClose(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler methods
    void OnLoadStart(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     TransitionType transition_type) override;
    void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   int httpStatusCode) override;
    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                              bool isLoading,
                              bool canGoBack,
                              bool canGoForward) override;
    void OnLoadError(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     ErrorCode errorCode,
                     const CefString& errorText,
                     const CefString& failedUrl) override;

    // CefRequestHandler methods
    bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefRequest> request,
                        bool user_gesture,
                        bool is_redirect) override;
    bool OnOpenURLFromTab(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            const CefString& target_url,
            CefRequestHandler::WindowOpenDisposition target_disposition,
            bool user_gesture) override;
    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request,
            bool is_navigation,
            bool is_download,
            const CefString& request_initiator,
            bool& disable_default_handling) override;
    bool GetAuthCredentials(CefRefPtr<CefBrowser> browser,
                            const CefString& origin_url,
                            bool isProxy,
                            const CefString& host,
                            int port,
                            const CefString& realm,
                            const CefString& scheme,
                            CefRefPtr<CefAuthCallback> callback) override;
    bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                        const CefString& origin_url,
                        int64 new_size,
#if CHROME_VERSION_MAJOR > 94
                        CefRefPtr<CefCallback> callback) override;
#else
                        CefRefPtr<CefRequestCallback> callback) override;
#endif
    bool OnCertificateError(CefRefPtr<CefBrowser> browser,
                            ErrorCode cert_error,
                            const CefString& request_url,
                            CefRefPtr<CefSSLInfo> ssl_info,
#if CHROME_VERSION_MAJOR > 94
                            CefRefPtr<CefCallback> callback) override;
#else
                            CefRefPtr<CefRequestCallback> callback) override;
#endif
    bool OnSelectClientCertificate(
            CefRefPtr<CefBrowser> browser,
            bool isProxy,
            const CefString& host,
            int port,
            const X509CertificateList& certificates,
            CefRefPtr<CefSelectClientCertificateCallback> callback) override;
    void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                   TerminationStatus status) override;
    void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser) override;

    // CefResourceRequestHandler methods
    cef_return_value_t OnBeforeResourceLoad(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request,
#if CHROME_VERSION_MAJOR > 94
            CefRefPtr<CefCallback> callback) override;
#else
            efRefPtr<CefRequestCallback> callback) override;
#endif
    CefRefPtr<CefResourceHandler> GetResourceHandler(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request) override;
    CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request,
            CefRefPtr<CefResponse> response) override;
    void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefRequest> request,
                             bool& allow_os_execution) override;

    // Show a new DevTools popup window.
    void ShowDevTools(CefRefPtr<CefBrowser> browser,
                      const CefPoint& inspect_element_at);

    // Close the existing DevTools popup window, if any.
    void CloseDevTools(CefRefPtr<CefBrowser> browser);

    // Test if the current site has SSL information available.
    bool HasSSLInformation(CefRefPtr<CefBrowser> browser);

    // Show SSL information for the current site.
    void ShowSSLInformation(CefRefPtr<CefBrowser> browser);

    // Set a string resource for loading via StringResourceProvider.
    void SetStringResource(const std::string& page, const std::string& data);

    // Returns the Delegate.
    QSharedPointer<Delegate> delegate() const { return m_delegate; }

    // Returns true if this handler uses off-screen rendering.
    bool is_osr() const { return m_is_osr_; }

    // Set/get whether the client should download favicon images. Only safe to
    // call immediately after client creation or on the browser process UI thread.
    bool download_favicon_images() const { return m_download_favicon_images_; }
    void set_download_favicon_images(bool allow)
    {
        m_download_favicon_images_ = allow;
    }

private:
    friend class ClientDownloadImageCallback;

    // Create a new popup window using the specified information. |is_devtools|
    // will be true if the window will be used for DevTools. Return true to
    // proceed with popup browser creation or false to cancel the popup browser.
    // May be called on any thead.
    bool CreatePopupWindow(CefRefPtr<CefBrowser> browser,
                           bool is_devtools,
                           CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                           const CefPopupFeatures& popupFeatures,
                           CefWindowInfo& windowInfo,
                           CefRefPtr<CefClient>& client,
                           CefBrowserSettings& settings);

    // Execute Delegate notifications on the main thread.
    void NotifyBrowserCreated(CefRefPtr<CefBrowser> browser);
    void NotifyBrowserClosing(CefRefPtr<CefBrowser> browser);
    void NotifyBrowserClosed(CefRefPtr<CefBrowser> browser);
    void NotifyAddress(CefRefPtr<CefBrowser> browser, const CefString& url);
    void NotifyTitle(CefRefPtr<CefBrowser> browser, const CefString& title);
    void NotifyFavicon(CefRefPtr<CefBrowser> browser, CefRefPtr<CefImage> image);
    void NotifyFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen);
    void NotifyAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size);
    void NotifyLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward);
    void NotifyDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions);
    void NotifyTakeFocus(CefRefPtr<CefBrowser> browser, bool next);

    // Test context menu creation.
    void BuildTestMenu(CefRefPtr<CefMenuModel> model);
    bool ExecuteTestMenu(int command_id);

    void SetOfflineState(CefRefPtr<CefBrowser> browser, bool offline);

    // THREAD SAFE MEMBERS
    // The following members may be accessed from any thread.

    // True if this handler uses off-screen rendering.
    const bool m_is_osr_;

    // True if mouse cursor change is disabled.
    bool m_mouse_cursor_change_disabled_;

    // True if the browser is currently offline.
    bool m_offline_;

    // True if Favicon images should be downloaded.
    bool m_download_favicon_images_;

#if defined(OS_LINUX)
    // Custom dialog handler for GTK.
//    CefRefPtr<ClientDialogHandlerGtk> dialog_handler_;
#endif

    // Handles the browser side of query routing. The renderer side is handled
    // in client_renderer.cc.
    CefRefPtr<CefMessageRouterBrowserSide> m_message_router_;

    // Manages the registration and delivery of resources.
    CefRefPtr<CefResourceManager> m_resource_manager_;

    // Used to manage string resources in combination with StringResourceProvider.
    // Only accessed on the IO thread.
    std::map<std::string, std::string> m_string_resource_map_;

    // MAIN THREAD MEMBERS
    // The following members will only be accessed on the main thread. This will
    // be the same as the CEF UI thread except when using multi-threaded message
    // loop mode on Windows.
    QSharedPointer<Delegate> m_delegate;

    // UI THREAD MEMBERS
    // The following members will only be accessed on the CEF UI thread.

    // Track state information for the text context menu.
    struct TestMenuState
    {
        TestMenuState() : check_item(true), radio_item(0) {}
        bool check_item;
        int radio_item;
    } m_test_menu_state_;

    CefRefPtr<CefDialogHandler> m_fileDialogHandler;
    CefRefPtr<CefJSDialogHandler> m_jsDialogHandler;

    // True if an editable field currently has focus.
    bool m_focus_on_editable_field_;

    // True for the initial navigation after browser creation.
    bool m_initial_navigation_;

    // Set of Handlers registered with the message router.
    std::set<CefMessageRouterBrowserSide::Handler*> m_message_handler_set_;

    IMPLEMENT_REFCOUNTING(ClientHandler);
    DISALLOW_COPY_AND_ASSIGN(ClientHandler);
};

}  // namespace QCefKits

#endif // CLIENTHANDLER_H

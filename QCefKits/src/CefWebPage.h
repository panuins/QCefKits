/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CEFWEBPAGE_H
#define CEFWEBPAGE_H
#include "ClientHandler.h"
#include <include/cef_client.h>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QUrl>
#include <QWidget>

class QCefWidget;
#ifdef Q_OS_LINUX
//#define LINUX_USING_X11_AS_MIDDLE_WINDOW
#define LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
#include <QWindow>
#endif
#endif

class CefWebPage : public QObject, public QCefKits::ClientHandler::Delegate
{
    Q_OBJECT
public:
    struct PageFeatures
    {
        QPointer<CefWebPage> parentPage;
        int x;
        bool xSet;
        int y;
        bool ySet;
        int width;
        bool widthSet;
        int height;
        bool heightSet;
        bool menuBarVisible;
        bool statusBarVisible;
        bool toolBarVisible;
        bool scrollbarsVisible;
        bool isPopup;
    };
    ~CefWebPage() override;

    static QSharedPointer<CefWebPage> createNewPage(QCefWidget *w);

    void closeBrowser();
    void createBrowser(const QUrl &url);
    void loadUrl(const QUrl &url);
    bool hasBrowser() const;

    // Called when the browser is created.
    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;

    // Called when the browser is closing.
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;

    // Called when the browser has been closed.
    virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) override;

    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                  cef_log_severity_t level,
                                  const CefString& message,
                                  const CefString& source,
                                  int line) override;
    // Set the window URL address.
    virtual void OnSetAddress(CefRefPtr<CefBrowser> browser, const std::string& url) override;

    // Set the window title.
    virtual void OnSetTitle(CefRefPtr<CefBrowser> browser, const std::string& title) override;

    // Set the Favicon image.
    virtual void OnSetFavicon(CefRefPtr<CefBrowser> browser, CefRefPtr<CefImage> image) override;

    // Set fullscreen mode.
    virtual void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) override;

    // Auto-resize contents.
    virtual void OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size) override;

    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefLoadHandler::TransitionType transition_type) override;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           int httpStatusCode) override;
    // Set the loading state.
    virtual void OnSetLoadingState(CefRefPtr<CefBrowser> browser,
                                   bool isLoading,
                                   bool canGoBack,
                                   bool canGoForward) override;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefLoadHandler::ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl) override;

    // Set the draggable regions.
    virtual void OnSetDraggableRegions(
            CefRefPtr<CefBrowser> browser,
            const std::vector<CefDraggableRegion>& regions) override;

    // Set focus to the next/previous control.
    virtual void OnGotFocus(CefRefPtr<CefBrowser> browser) override;
    virtual bool OnSetFocus(CefRefPtr<CefBrowser> browser, CefFocusHandler::FocusSource source) override;
    virtual void OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) override;

    // Called on the UI thread before a context menu is displayed.
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model) override;

    virtual QSharedPointer<QCefKits::ClientHandler::Delegate> CreatePopupWindow(
            CefRefPtr<CefBrowser> browser,
            bool is_devtools,
            CefLifeSpanHandler::WindowOpenDisposition target_disposition,
            const CefPopupFeatures& popupFeatures,
            CefWindowInfo& windowInfo,
            CefBrowserSettings& settings) override;

    void setFocus(bool f);
    void resizeBrowser(const QSize &size);
    QPointer<QCefWidget> cefWidget() const
    {
        return m_widget;
    }
    void setCefWidget(QPointer<QCefWidget> widget);
    void show();
    void hide();

    struct PageFeatures pageFeatures;

    CefRefPtr<CefBrowser> m_browser;

Q_SIGNALS:
    void urlChanged(QUrl url);
    void titleChanged(QString title);
    void browserCreated();
    void browserClosed();
    void loadStarted();
    void loadComplete();
    void loadFailed(int errorCode,
                    QString errorText,
                    QUrl failedUrl);
    void loadingStateChanged(bool isLoading,
                             bool canGoBack,
                             bool canGoForward);
    void newBrowserRequest(QSharedPointer<CefWebPage> page);
    void consoleMessage(QCefKits_logLevel level,
                        QString message,
                        QString source,
                        int line);

private:
    explicit CefWebPage();

    CefRefPtr<QCefKits::ClientHandler> m_handler;
    QPointer<QCefWidget> m_widget;
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
    QPointer<QWindow> m_parentWindow;
    QPointer<QWindow> m_middleWindow;
    QPointer<QWindow> m_cefWindow;
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
    WId m_parentWinid = 0;
#endif

    friend class QCefWidget;
};

#endif // CEFWEBPAGE_H

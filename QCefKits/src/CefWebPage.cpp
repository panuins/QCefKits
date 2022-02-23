/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "CefWebPage.h"
#include "include/QCefWidget.h"
#include "ClientHandler.h"
#include "ClientRequestContextHandler.h"
#include "ClientUtils.h"
#include <include/cef_browser.h>
#include <include/views/cef_browser_view.h>
#include <include/views/cef_window.h>
#include <include/views/cef_view.h>
#include <QApplication>
#include <QDebug>
#ifdef Q_OS_WIN
#include <Windows.h>
#elif defined(Q_OS_LINUX)
#include "X11Utils.h"
#endif
#undef Bool
#undef Status

CefWebPage::CefWebPage()
    : QObject(nullptr)
{
}

CefWebPage::~CefWebPage()
{
    qDebug() << "CefWebPage::~CefWebPage";
    closeBrowser();
#ifdef Q_OS_LINUX
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
    if (m_parentWindow)
    {
        m_parentWindow->deleteLater();
    }
    if (m_cefWindow)
    {
        m_cefWindow->deleteLater();
    }
    if (m_middleWindow)
    {
        m_middleWindow->close();
        m_middleWindow->deleteLater();
    }
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
    if (m_parentWinid)
    {
        QCefKits::DestoryCefBrowserWindow(m_parentWinid);
        m_parentWinid = 0;
    }
#endif
#endif
}

void CefWebPage::closeBrowser()
{
//    qDebug() << "CefWebPage::closeBrowser";
#ifdef Q_OS_LINUX
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
//    if (m_parentWindow)
//    {
//        m_parentWindow->setParent(nullptr);
//    }
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
    if (m_parentWinid)
    {
//        QCefKits::ReparentWindow(0, m_parentWinid, 0);
//        XUnmapWindow(QX11Info::display(), m_parentWinid);
    }
#endif
#endif
    if (m_browser && (m_browser->GetHost()))
    {
#ifdef Q_OS_LINUX
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
//        QCefKits::ReparentWindow(0, m_browser->GetHost()->GetWindowHandle(), 0);
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
        QCefKits::ReparentWindow(0, m_browser->GetHost()->GetWindowHandle(), 0);
//        XUnmapWindow(QX11Info::display(), m_browser->GetHost()->GetWindowHandle());
#endif
#endif
//        qDebug() << "CefWebPage::closeBrowser a";
        m_browser->GetHost()->CloseBrowser(false);
//        m_browser = nullptr;
    }
//    qDebug() << "CefWebPage::closeBrowser end";
}

void CefWebPage::createBrowser(const QUrl &url)
{
    CefWindowInfo window_info;
#ifdef Q_OS_WIN
#if CHROME_VERSION_MAJOR > 94
    CefRect wnd_rect = {0, 0, m_widget->width(), m_widget->height()};
    window_info.SetAsChild(HWND(m_widget->winId()), wnd_rect);
#else
    RECT wnd_rect = {0, 0, m_widget->width(), m_widget->height()};
    window_info.SetAsChild(HWND(m_widget->winId()), wnd_rect);
#endif

//    if (GetWindowLongPtr(m_widget->winId(), GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
//      // Don't activate the browser window on creation.
//      window_info.ex_style |= WS_EX_NOACTIVATE;
//    }

#elif defined(Q_OS_LINUX)
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
    if (!m_middleWindow.isNull())
    {
        CefRect wnd_rect = {0, 0, 100, 100};
        window_info.SetAsChild(static_cast<CefWindowHandle>(m_middleWindow->winId()), wnd_rect);
    }
    else
    {
        qDebug() << "CefWebPage::createBrowser: error: m_middleWindow.isNull, you should init this page first.";
    }
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
    m_parentWinid = QCefKits::CreateCefBrowserWindow(100, 100);
    CefRect wnd_rect = {0, 0, 100, 100};
    window_info.SetAsChild(static_cast<CefWindowHandle>(m_parentWinid), wnd_rect);
#endif
#endif
    CefString startUpUrl = url.toString().toStdWString();
//    qDebug() << "CefWebPage::createBrowser" << url;
    CefBrowserHost::CreateBrowser(window_info, m_handler,
                                  startUpUrl, CefBrowserSettings(),
                                  nullptr, CefRequestContext::CreateContext(
                                      CefRequestContext::GetGlobalContext(), new QCefKits::ClientRequestContextHandler));
}

void CefWebPage::loadUrl(const QUrl &url)
{
    if (m_browser)
    {
        if (m_browser->GetMainFrame())
        {
            std::wstring u = url.toString().toStdWString();
            m_browser->GetMainFrame()->LoadURL(u);
        }
        else
        {
            qDebug() << "CefWebPage::loadUrl: error: m_browser->GetMainFrame() is null";
        }
    }
    else
    {
        qDebug() << "CefWebPage::loadUrl: error: m_browser is null";
    }
}

bool CefWebPage::hasBrowser() const
{
    return m_browser.get() != nullptr;
}

QSharedPointer<CefWebPage> CefWebPage::createNewPage(QCefWidget *w)
{
    QSharedPointer<CefWebPage> ret(new CefWebPage());
    ret->m_widget = w;
    ret->m_handler = new QCefKits::ClientHandler(ret, false);
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
    ret->m_middleWindow = new QWindow();
    ret->m_parentWindow = QWindow::fromWinId(w->winId());
//    ret->m_middleWindow->show();
#endif
    return ret;
}

// Called when the browser is created.
void CefWebPage::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
//    qDebug() << "CefWebPage::OnBrowserCreated"
//             << browser->GetIdentifier();
    m_browser = browser;
    WId handler = WId(m_browser->GetHost()->GetWindowHandle());
#ifdef Q_OS_WIN
    ::MoveWindow(HWND(handler), 0, 0, m_widget->width(), m_widget->height(), true);
#elif defined(Q_OS_LINUX)
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
    if (m_middleWindow)
    {
        m_middleWindow->show();
        m_middleWindow->setParent(m_parentWindow);
        m_middleWindow->setX(0);
        m_middleWindow->setY(0);
        m_middleWindow->resize(m_widget->size());
    }
    m_cefWindow = QWindow::fromWinId(handler);
    m_cefWindow->resize(m_widget->size());
//    m_cefWindow->setParent(m_middleWindow);
//    m_cefWindow->show();
//    QCefKits::SetXWindowBounds(handler, 0, 0, m_widget->width(), m_widget->height());
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
    QCefKits::ReparentWindow(m_widget->winId(), m_parentWinid, 0);
    QCefKits::SetXWindowBounds(m_parentWinid, 0, 0, m_widget->width(), m_widget->height());
    QCefKits::SetXWindowBounds(handler, 0, 0, m_widget->width(), m_widget->height());
#endif
#endif
    m_browser->GetHost()->NotifyMoveOrResizeStarted();
    emit browserCreated();
}

// Called when the browser is closing.
bool CefWebPage::DoClose(CefRefPtr<CefBrowser> browser)
{
//    qDebug() << "CefWebPage::DoClose" << browser->GetIdentifier();
    if (m_widget)
    {
        m_widget->setParent(nullptr);
    }
//#ifdef Q_OS_LINUX
//#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
//    QCefKits::ReparentWindow(0, m_browser->GetHost()->GetWindowHandle(), 0);
//    if (m_parentWidget)
//    {
//        delete m_parentWidget.data();
//    }
//    if (m_parentWindow)
//    {
//        delete m_parentWindow.data();
//    }
//#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
//    QCefKits::ReparentWindow(0, m_browser->GetHost()->GetWindowHandle(), 0);
//    if (m_parentWinid)
//    {
//        QCefKits::ReparentWindow(0, m_parentWinid, 0);
//        QCefKits::DestoryCefBrowserWindow(m_parentWinid);
//        m_parentWinid = 0;
//    }
//#endif
//#endif
//    emit browserClosed();
    m_browser = nullptr;
    m_handler = nullptr;
    return false;
}

// Called when the browser has been closed.
void CefWebPage::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
{
//    qDebug() << "CefWebPage::OnBrowserClosed" << browser->GetIdentifier();
    m_browser = nullptr;
    m_handler = nullptr;
    emit browserClosed();
}

bool CefWebPage::OnConsoleMessage(CefRefPtr<CefBrowser> /*browser*/,
                                  cef_log_severity_t level,
                                  const CefString& message,
                                  const CefString& source,
                                  int line)
{
    emit consoleMessage(QCefKits::CefLogServerityToLogLevel(level),
                        QString::fromStdWString(message.ToWString()),
                        QString::fromStdWString(source.ToWString()),
                        line);
    return false;
}

// Set the window URL address.
void CefWebPage::OnSetAddress(CefRefPtr<CefBrowser> /*browser*/, const std::string& url)
{
    emit urlChanged(QString::fromStdString(url));
}

// Set the window title.
void CefWebPage::OnSetTitle(CefRefPtr<CefBrowser> /*browser*/, const std::string& title)
{
    emit titleChanged(QString::fromStdString(title));
}

// Set the Favicon image.
void CefWebPage::OnSetFavicon(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefImage> /*image*/)
{
    //
}

// Set fullscreen mode.
void CefWebPage::OnSetFullscreen(CefRefPtr<CefBrowser> /*browser*/, bool /*fullscreen*/)
{
    //
}

// Auto-resize contents.
void CefWebPage::OnAutoResize(CefRefPtr<CefBrowser> /*browser*/, const CefSize& /*new_size*/)
{
    //
}

void CefWebPage::OnLoadStart(CefRefPtr<CefBrowser> /*browser*/,
                             CefRefPtr<CefFrame> /*frame*/,
                             CefLoadHandler::TransitionType /*transition_type*/)
{
//    qDebug() << "CefWebPage::OnLoadStart";
    emit loadStarted();
}

void CefWebPage::OnLoadEnd(CefRefPtr<CefBrowser> /*browser*/,
                           CefRefPtr<CefFrame> /*frame*/,
                           int httpStatusCode)
{
//    qDebug() << "CefWebPage::OnLoadEnd" << httpStatusCode;
    if ((httpStatusCode >= 100) && (httpStatusCode < 400))
    {
        emit loadComplete();
    }
}

// Set the loading state.
void CefWebPage::OnSetLoadingState(CefRefPtr<CefBrowser> /*browser*/,
                                   bool isLoading,
                                   bool canGoBack,
                                   bool canGoForward)
{
//    qDebug() << "CefWebPage::OnSetLoadingState" << browser->GetIdentifier()
//             << isLoading;
    emit loadingStateChanged(isLoading, canGoBack, canGoForward);
}

void CefWebPage::OnLoadError(CefRefPtr<CefBrowser> /*browser*/,
                             CefRefPtr<CefFrame> /*frame*/,
                             CefLoadHandler::ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl)
{
//    qDebug() << "CefWebPage::OnLoadError" << QString::fromStdWString(errorText.ToWString());
    emit loadFailed(int(errorCode),
                    QString::fromStdWString(errorText.ToWString()),
                    QString::fromStdWString(failedUrl.ToWString()));
}

// Set the draggable regions.
void CefWebPage::OnSetDraggableRegions(
        CefRefPtr<CefBrowser> /*browser*/,
        const std::vector<CefDraggableRegion>& /*regions*/)
{
    //
}

void CefWebPage::OnGotFocus(CefRefPtr<CefBrowser> /*browser*/)
{
    if (qApp)
    {
        if (qApp->focusWidget() && qApp->focusWidget() != m_widget.data())
        {
            qApp->focusWidget()->clearFocus();
        }
    }
}

bool CefWebPage::OnSetFocus(CefRefPtr<CefBrowser> /*browser*/, CefFocusHandler::FocusSource /*source*/)
{
    return false;
}

// Set focus to the next/previous control.
void CefWebPage::OnTakeFocus(CefRefPtr<CefBrowser> /*browser*/, bool /*next*/)
{
    //
}

// Called on the UI thread before a context menu is displayed.
void CefWebPage::OnBeforeContextMenu(CefRefPtr<CefBrowser> /*browser*/,
                                     CefRefPtr<CefFrame> /*frame*/,
                                     CefRefPtr<CefContextMenuParams> /*params*/,
                                     CefRefPtr<CefMenuModel> /*model*/)
{
    //
}

QSharedPointer<QCefKits::ClientHandler::Delegate> CefWebPage::CreatePopupWindow(
        CefRefPtr<CefBrowser> /*browser*/,
        bool /*is_devtools*/,
        CefLifeSpanHandler::WindowOpenDisposition target_disposition,
        const CefPopupFeatures& popupFeatures,
        CefWindowInfo& windowInfo,
        CefBrowserSettings& settings)
{
//    qDebug() << "CefWebPage::CreatePopupWindow begin";
    QSharedPointer<CefWebPage> ret(new CefWebPage());
    ret->pageFeatures.parentPage = this;
    ret->pageFeatures.x = popupFeatures.x;
    ret->pageFeatures.xSet = popupFeatures.xSet!=0;
    ret->pageFeatures.y = popupFeatures.y;
    ret->pageFeatures.ySet = popupFeatures.ySet!=0;
    ret->pageFeatures.width = popupFeatures.width;
    ret->pageFeatures.widthSet = popupFeatures.widthSet!=0;
    ret->pageFeatures.height = popupFeatures.height;
    ret->pageFeatures.heightSet = popupFeatures.heightSet!=0;
    ret->pageFeatures.menuBarVisible = popupFeatures.menuBarVisible;
    ret->pageFeatures.statusBarVisible = popupFeatures.statusBarVisible;
    ret->pageFeatures.toolBarVisible = popupFeatures.toolBarVisible;
    ret->pageFeatures.scrollbarsVisible = popupFeatures.scrollbarsVisible;
    ret->pageFeatures.isPopup = (target_disposition == WOD_NEW_POPUP)
            || (target_disposition == WOD_NEW_WINDOW);
    qDebug() << "CefWebPage::CreatePopupWindow before newBrowserRequest"
             << target_disposition;
    emit newBrowserRequest(ret);
//    qDebug() << "CefWebPage::CreatePopupWindow after newBrowserRequest"
//             << target_disposition << ret->m_widget;
    if (ret->m_widget)
    {
        ret->moveToThread(ret->m_widget->thread());
#ifdef Q_OS_WIN
#if CHROME_VERSION_MAJOR > 94
    CefRect wnd_rect = {0, 0, ret->m_widget->width(), ret->m_widget->height()};
    windowInfo.SetAsChild(HWND(ret->m_widget->winId()), wnd_rect);
#else
    RECT wnd_rect = {0, 0, ret->m_widget->width(), ret->m_widget->height()};
    windowInfo.SetAsChild(HWND(ret->m_widget->winId()), wnd_rect);
#endif
#elif defined(Q_OS_LINUX)
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
//        ret->m_parentWindow = QWindow::fromWinId(ret->m_widget->winId());
//        ret->m_middleWindow = new QWindow;
        CefRect wnd_rect = {0, 0, 100, 100};
        windowInfo.SetAsChild(static_cast<CefWindowHandle>(ret->m_middleWindow->winId()), wnd_rect);
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
        ret->m_parentWinid = QCefKits::CreateCefBrowserWindow(100, 100);
        CefRect wnd_rect = {0, 0, 100, 100};
        windowInfo.SetAsChild(static_cast<CefWindowHandle>(ret->m_parentWinid), wnd_rect);
#endif
#endif
    }
    return ret.staticCast<QCefKits::ClientHandler::Delegate>();
}

void CefWebPage::setFocus(bool f)
{
    if (m_browser)
    {
        if (m_browser->GetHost())
        {
            m_browser->GetHost()->SetFocus(f);
        }
    }
}

void CefWebPage::resizeBrowser(const QSize &size)
{
    if (m_browser != nullptr)
    {
        WId handler = WId(m_browser->GetHost()->GetWindowHandle());
#ifdef Q_OS_WIN
        ::MoveWindow(HWND(handler), 0, 0, size.width(), size.height(), true);
#elif defined(Q_OS_LINUX)
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
        (void)handler;
        if (m_middleWindow)
        {
            m_middleWindow->resize(size);
        }
        if (m_cefWindow)
        {
            m_cefWindow->resize(size);
        }
//        QCefKits::SetXWindowBounds(handler, 0, 0, size.width(), size.height());
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
        QCefKits::SetXWindowBounds(m_parentWinid, 0, 0, size.width(), size.height());
        QCefKits::SetXWindowBounds(handler, 0, 0, size.width(), size.height());
#endif
//        qDebug() << "CefWebPage::resizeBrowser" << size;
#endif
        m_browser->GetHost()->NotifyMoveOrResizeStarted();
    }
}

void CefWebPage::setCefWidget(QPointer<QCefWidget> widget)
{
#ifdef Q_OS_WIN
#elif defined(Q_OS_LINUX)
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
    if (m_parentWindow)
    {
        delete m_parentWindow.data();
    }
    m_parentWindow = QWindow::fromWinId(widget->winId());
#elif defined(LINUX_USING_X11_AS_MIDDLE_WINDOW)
    if (widget)
    {
        QCefKits::ReparentWindow(widget->winId(), m_parentWinid, 0);
    }
#endif
#endif
    m_widget = widget;
}

void CefWebPage::show()
{
    if (m_browser)
    {
#ifdef Q_OS_WIN
        HWND hwnd = m_browser->GetHost()->GetWindowHandle();
        if (hwnd && !::IsWindowVisible(hwnd))
        {
            ShowWindow(hwnd, SW_SHOW);
        }
#endif
    }
}

void CefWebPage::hide()
{
    if (m_browser)
    {
#ifdef Q_OS_WIN
        HWND hwnd = m_browser->GetHost()->GetWindowHandle();
        if (hwnd && !::IsWindowVisible(hwnd))
        {
            ShowWindow(hwnd, SW_HIDE);
        }
#endif
    }
}

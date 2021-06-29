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
#include <QDebug>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

CefWebPage::CefWebPage()
    : QObject(nullptr)
{

}

CefWebPage::~CefWebPage()
{
    if (m_browser)
    {
        m_browser->GetHost()->CloseBrowser(true);
    }
}

void CefWebPage::createBrowser(const QUrl &url)
{
#ifdef Q_OS_WIN
    CefWindowInfo window_info;
    RECT wnd_rect = {0, 0, m_widget->width(), m_widget->height()};
    window_info.SetAsChild(HWND(m_widget->winId()), wnd_rect);

//    if (GetWindowLongPtr(m_widget->winId(), GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
//      // Don't activate the browser window on creation.
//      window_info.ex_style |= WS_EX_NOACTIVATE;
//    }

    CefString startUpUrl = url.toString().toStdWString();
    CefBrowserHost::CreateBrowser(window_info, m_handler,
                                  startUpUrl, CefBrowserSettings(),
                                  nullptr, CefRequestContext::CreateContext(
                                      CefRequestContext::GetGlobalContext(), new CefHandler::ClientRequestContextHandler));
#endif
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

QSharedPointer<CefWebPage> CefWebPage::createNewPage(QCefWidget *w, const QUrl &url)
{
    QSharedPointer<CefWebPage> ret(new CefWebPage());
    ret->m_widget = w;
    std::wstring strurl;
    if (url.isValid())
    {
        strurl = url.toString().toStdWString();
    }
    ret->m_handler = new CefHandler::ClientHandler(ret, false, strurl);
    return ret;
}

// Called when the browser is created.
void CefWebPage::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
//    qDebug() << "CefWebPage::OnBrowserCreated"
//             << browser->GetIdentifier();
    m_browser = browser;
    emit browserCreated();
}

// Called when the browser is closing.
void CefWebPage::OnBrowserClosing(CefRefPtr<CefBrowser> /*browser*/)
{
    //
}

// Called when the browser has been closed.
void CefWebPage::OnBrowserClosed(CefRefPtr<CefBrowser> /*browser*/)
{
    emit browserClosed();
}

bool CefWebPage::OnConsoleMessage(CefRefPtr<CefBrowser> /*browser*/,
                                  cef_log_severity_t level,
                                  const CefString& message,
                                  const CefString& source,
                                  int line)
{
    emit consoleMessage(CefHandler::CefLogServerityToLogLevel(level),
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
    emit loadStarted();
}

void CefWebPage::OnLoadEnd(CefRefPtr<CefBrowser> /*browser*/,
                           CefRefPtr<CefFrame> /*frame*/,
                           int httpStatusCode)
{
    if ((httpStatusCode >= 100) && (httpStatusCode < 400))
    {
        emit loadComplete();
    }
}

// Set the loading state.
void CefWebPage::OnSetLoadingState(CefRefPtr<CefBrowser> /*browser*/,
                                   bool /*isLoading*/,
                                   bool /*canGoBack*/,
                                   bool /*canGoForward*/)
{
    //
}

void CefWebPage::OnLoadError(CefRefPtr<CefBrowser> /*browser*/,
                             CefRefPtr<CefFrame> /*frame*/,
                             CefLoadHandler::ErrorCode /*errorCode*/,
                             const CefString& /*errorText*/,
                             const CefString& /*failedUrl*/)
{
    emit loadFailed();
}

// Set the draggable regions.
void CefWebPage::OnSetDraggableRegions(
        CefRefPtr<CefBrowser> /*browser*/,
        const std::vector<CefDraggableRegion>& /*regions*/)
{
    //
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

QSharedPointer<CefHandler::ClientHandler::Delegate> CefWebPage::CreatePopupWindow(
        CefRefPtr<CefBrowser> /*browser*/,
        bool /*is_devtools*/,
        CefLifeSpanHandler::WindowOpenDisposition target_disposition,
        const CefPopupFeatures& popupFeatures,
        CefWindowInfo& windowInfo,
        CefBrowserSettings& settings)
{
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
    emit newBrowserRequest(ret);
//    qDebug() << "CefWebPage::CreatePopupWindow after newBrowserRequest";
    if (ret->m_widget)
    {
        ret->moveToThread(ret->m_widget->thread());
#ifdef Q_OS_WIN
        RECT wnd_rect = {0, 0, ret->m_widget->width(), ret->m_widget->height()};
        windowInfo.SetAsChild(HWND(ret->m_widget->winId()), wnd_rect);
#endif
    }
    return ret.staticCast<CefHandler::ClientHandler::Delegate>();
}

void CefWebPage::resizeBrowser(const QSize &size)
{
    if (m_browser != nullptr)
    {
        WId handler = WId(m_browser->GetHost()->GetWindowHandle());
#ifdef Q_OS_WIN
        ::MoveWindow(HWND(handler), 0, 0, size.width(), size.height(), true);
#elif defined(Q_OS_LINUX)
#endif
        m_browser->GetHost()->NotifyMoveOrResizeStarted();
    }
}

void CefWebPage::setCefWidget(QPointer<QCefWidget> widget)
{
    m_widget = widget;
}

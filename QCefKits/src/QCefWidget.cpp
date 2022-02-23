/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "include/QCefWidget.h"
#include "src/CefWebPage.h"
#include "QCefKits_internal.h"
#include <QApplication>
#include <QTimer>
#include <QThread>
#include <QResizeEvent>
#include <QDebug>

QCefWidget::QCefWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setAttribute(Qt::WA_NativeWindow, true);
#if defined(Q_OS_LINUX)
    this->setAttribute(Qt::WA_NativeWindow, true);
    this->setAttribute(Qt::WA_DontCreateNativeAncestors, true);
#endif
    winId();
}

QCefWidget::~QCefWidget()
{
    qDebug() << "QCefWidget::~QCefWidget";
    closeBrowser();
    if (hasCefBrowser())
    {
        if (QCefKits::g_cefSettings.external_message_pump)
        {
            CefDoMessageLoopWork();
        }
        //QThread::msleep(10);
    }
}

void QCefWidget::closeBrowser()
{
    if (m_page)
    {
//        qDebug() << "QCefWidget::closeBrowser" << this;
        m_page->closeBrowser();
        m_page.clear();
    }
}

void QCefWidget::executeJs(
        const QString &code, const QUrl &scriptUrl, int startLine)
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetMainFrame())
            {
                std::wstring u = scriptUrl.toString().toStdWString();
                if (scriptUrl.isEmpty())
                {
                    u = m_page->m_browser->GetMainFrame()->GetURL().ToWString();
                }
                m_page->m_browser->GetMainFrame()->ExecuteJavaScript(
                            code.toStdWString(), u, startLine);
            }
        }
    }
}

void QCefWidget::init(const QUrl &url)
{
    setWebPage(CefWebPage::createNewPage(this));
    m_page->createBrowser(url);
    m_currentUrl = url;
}

bool QCefWidget::hasCefBrowser()
{
    if (m_page)
    {
        return m_page->hasBrowser();
    }
    return false;
}

void QCefWidget::loadUrl(const QUrl &url)
{
    m_page->loadUrl(url);
}

void QCefWidget::setBrowserFocus(bool f)
{
    if (m_page)
    {
        m_page->setFocus(f);
    }
}

void QCefWidget::setWebPage(QSharedPointer<CefWebPage> page)
{
    m_page = page;
//    connect(m_page.data(), SIGNAL(urlChanged(QUrl)),
//            this, SIGNAL(urlChanged(QUrl)));
    connect(m_page.data(), &CefWebPage::urlChanged,
            this, &QCefWidget::onUrlChanged);
    connect(m_page.data(), SIGNAL(titleChanged(QString)),
            this, SLOT(setWindowTitle(QString)));
    connect(m_page.data(), SIGNAL(titleChanged(QString)),
            this, SIGNAL(titleChanged(QString)));
    connect(m_page.data(), &CefWebPage::browserCreated,
            this, &QCefWidget::onBrowserCreated);
    connect(m_page.data(), &CefWebPage::browserClosed,
            this, &QCefWidget::onBrowserClosed);
    connect(m_page.data(), SIGNAL(loadStarted()),
            this, SIGNAL(loadStarted()));
    connect(m_page.data(), SIGNAL(loadComplete()),
            this, SIGNAL(loadComplete()));
    connect(m_page.data(), SIGNAL(loadFailed(int, QString, QUrl)),
            this, SIGNAL(loadFailed(int, QString, QUrl)));
    connect(m_page.data(), SIGNAL(loadingStateChanged(bool, bool, bool)),
            this, SIGNAL(loadingStateChanged(bool, bool, bool)));
    connect(m_page.data(), &CefWebPage::newBrowserRequest,
            this, &QCefWidget::onNewBrowserRequest,
    (QCefKits::g_cefSettings.multi_threaded_message_loop == 0) ? Qt::AutoConnection : Qt::BlockingQueuedConnection);
}

QUrl QCefWidget::url() const
{
    return m_currentUrl;
}

void QCefWidget::goBack()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            m_page->m_browser->GoBack();
        }
    }
}

void QCefWidget::goForward()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            m_page->m_browser->GoForward();
        }
    }
}

void QCefWidget::reload()
{
//    if (m_page)
//    {
//        if (m_page->m_browser)
//        {
//            m_page->m_browser->Reload();
//        }
//    }
    executeJs("location.reload();");
}

void QCefWidget::stopLoad()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            m_page->m_browser->StopLoad();
        }
    }
}

void QCefWidget::undo()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetFocusedFrame())
            {
                m_page->m_browser->GetFocusedFrame()->Undo();
            }
        }
    }
}

void QCefWidget::redo()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetFocusedFrame())
            {
                m_page->m_browser->GetFocusedFrame()->Redo();
            }
        }
    }
}

void QCefWidget::cut()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetFocusedFrame())
            {
                m_page->m_browser->GetFocusedFrame()->Cut();
            }
        }
    }
}

void QCefWidget::copy()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetFocusedFrame())
            {
                m_page->m_browser->GetFocusedFrame()->Copy();
            }
        }
    }
}

void QCefWidget::paste()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetFocusedFrame())
            {
                m_page->m_browser->GetFocusedFrame()->Paste();
            }
        }
    }
}

void QCefWidget::doDelete()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetFocusedFrame())
            {
                m_page->m_browser->GetFocusedFrame()->Delete();
            }
        }
    }
}

void QCefWidget::selectAll()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            if (m_page->m_browser->GetFocusedFrame())
            {
                m_page->m_browser->GetFocusedFrame()->SelectAll();
            }
        }
    }
}

bool QCefWidget::canGoBack()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            return m_page->m_browser->CanGoBack();
        }
    }
    return false;
}

bool QCefWidget::canGoForward()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            return m_page->m_browser->CanGoForward();
        }
    }
    return false;
}

bool QCefWidget::isLoading()
{
    if (m_page)
    {
        if (m_page->m_browser)
        {
            return m_page->m_browser->IsLoading();
        }
    }
    return false;
}


void QCefWidget::closeEvent(QCloseEvent *event)
{
//    qDebug() << "Widget::closeEvent 1";
    if (hasCefBrowser())
    {
        closeBrowser();
    }
    while (hasCefBrowser())
    {
        if (QCefKits::g_cefSettings.external_message_pump)
        {
            CefDoMessageLoopWork();
        }
        QThread::msleep(10);
    }
//    else
    QWidget::closeEvent(event);
//    qDebug() << "Widget::closeEvent 2";
//    qDebug() << "Widget::closeEvent 3";
}

void QCefWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_page.isNull())
    {
        m_page->resizeBrowser(event->size());
    }
}

void QCefWidget::focusInEvent(QFocusEvent *event)
{
    qDebug() << "QCefWidget::focusInEvent";
    QWidget::focusInEvent(event);
    setBrowserFocus(true);
}

void QCefWidget::focusOutEvent(QFocusEvent *event)
{
    qDebug() << "QCefWidget::focusOutEvent";
    QWidget::focusOutEvent(event);
    setBrowserFocus(false);
}

void QCefWidget::enterEvent(QEvent *event)
{
//    qDebug() << "QCefWidget::enterEvent";
    QWidget::enterEvent(event);
    setBrowserFocus(true);
}

void QCefWidget::leaveEvent(QEvent *event)
{
//    qDebug() << "QCefWidget::leaveEvent";
    QWidget::leaveEvent(event);
    setBrowserFocus(false);
}

void QCefWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void QCefWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

void QCefWidget::onBrowserCreated()
{
    m_page->resizeBrowser(size());
    emit browserCreated();
    QPointer<CefWebPage> parentPage = m_page->pageFeatures.parentPage;
//    qDebug() << "QCefWidget::onBrowserCreated" << parentPage;
    if (!parentPage.isNull())
    {
        parentPage->cefWidget()->newBrowserCreated(
                    this,
                    m_page->pageFeatures.x,
                    m_page->pageFeatures.xSet,
                    m_page->pageFeatures.y,
                    m_page->pageFeatures.ySet,
                    m_page->pageFeatures.width,
                    m_page->pageFeatures.widthSet,
                    m_page->pageFeatures.height,
                    m_page->pageFeatures.heightSet,
                    m_page->pageFeatures.menuBarVisible,
                    m_page->pageFeatures.statusBarVisible,
                    m_page->pageFeatures.toolBarVisible,
                    m_page->pageFeatures.scrollbarsVisible,
                    m_page->pageFeatures.isPopup);
//        connect(&m_timer, &QTimer::timeout,
//                this, &QCefWidget::onTimerTimeout);
//        m_timer.start(10);
    }
}

void QCefWidget::onBrowserClosed()
{
    //setParent(nullptr);
//    qDebug() << "QCefWidget::onBrowserClosed" << this;
    emit browserClosed();
}

void QCefWidget::onTimerTimeout()
{
    m_timer.stop();
    QPointer<CefWebPage> parentPage = m_page->pageFeatures.parentPage;
    if (!parentPage.isNull())
    {
    }
}

void QCefWidget::onNewBrowserRequest(QSharedPointer<CefWebPage> page)
{
//        qDebug() << "CefWebPage::newBrowserRequest begin";
    QCefWidget *newBrowser = new QCefWidget;
    //These operation must done in main thread, or newBrowser can not set new parent.
    page->setCefWidget(newBrowser);
#ifdef LINUX_USING_QWINDOW_AS_MIDDLE_WINDOW
//        page->m_parentWindow = QWindow::fromWinId(newBrowser->winId());
    page->m_middleWindow = new QWindow;
    page->m_middleWindow->winId();
#endif
//        newBrowser->winId();
    newBrowser->setWebPage(page);
//        qDebug() << "CefWebPage::newBrowserRequest end";
//    this->newBrowserCreated(
//                newBrowser,
//                m_page->pageFeatures.x,
//                m_page->pageFeatures.xSet,
//                m_page->pageFeatures.y,
//                m_page->pageFeatures.ySet,
//                m_page->pageFeatures.width,
//                m_page->pageFeatures.widthSet,
//                m_page->pageFeatures.height,
//                m_page->pageFeatures.heightSet,
//                m_page->pageFeatures.menuBarVisible,
//                m_page->pageFeatures.statusBarVisible,
//                m_page->pageFeatures.toolBarVisible,
//                m_page->pageFeatures.scrollbarsVisible,
//                m_page->pageFeatures.isPopup);
}

void QCefWidget::onUrlChanged(QUrl url)
{
    m_currentUrl = url;
    emit urlChanged(url);
}

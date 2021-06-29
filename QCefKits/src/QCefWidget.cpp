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
#include <QApplication>
#include <QTimer>
#include <QThread>
#include <QResizeEvent>
#include <QDebug>

extern CefSettings g_cefSettings;

QCefWidget::QCefWidget(QWidget *parent)
    : QWidget(parent)
{
}

QCefWidget::~QCefWidget()
{
    closeBrowser();
}

void QCefWidget::init(const QUrl &url)
{
//    m_page = CefWebPage::createNewPage(this, url);
    setWebPage(CefWebPage::createNewPage(this, url));
    m_page->createBrowser(url);
}

void QCefWidget::setWebPage(QSharedPointer<CefWebPage> page)
{
    m_page = page;
    connect(m_page.data(), SIGNAL(urlChanged(QUrl)),
            this, SIGNAL(urlChanged(QUrl)));
    connect(m_page.data(), SIGNAL(titleChanged(QString)),
            this, SIGNAL(titleChanged(QString)));
    connect(m_page.data(), SIGNAL(titleChanged(QString)),
            this, SLOT(setWindowTitle(QString)));
    connect(m_page.data(), &CefWebPage::browserCreated,
            this, &QCefWidget::onBrowserCreated);
    connect(m_page.data(), SIGNAL(browserClosed()),
            this, SIGNAL(browserClosed()));
    connect(m_page.data(), SIGNAL(loadStarted()),
            this, SIGNAL(loadStarted()));
    connect(m_page.data(), SIGNAL(loadComplete()),
            this, SIGNAL(loadComplete()));
    connect(m_page.data(), SIGNAL(loadFailed()),
            this, SIGNAL(loadFailed()));
    connect(m_page.data(), &CefWebPage::newBrowserRequest,
            this, [](QSharedPointer<CefWebPage> page)
    {
        QCefWidget *newBrowser = new QCefWidget;
        //These operation must done in main thread, or newBrowser can not set new parent.
        page->setCefWidget(newBrowser);
        newBrowser->winId();
        newBrowser->setWebPage(page);
    },
    (g_cefSettings.multi_threaded_message_loop == 0) ? Qt::AutoConnection : Qt::BlockingQueuedConnection);
}

void QCefWidget::loadUrl(const QUrl &url)
{
    m_page->loadUrl(url);
}

void QCefWidget::closeBrowser()
{
    m_page.clear();
}

void QCefWidget::closeEvent(QCloseEvent *event)
{
//    qDebug() << "Widget::closeEvent 1";
    closeBrowser();
//    qDebug() << "Widget::closeEvent 2";
    QWidget::closeEvent(event);
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

void QCefWidget::onTimerTimeout()
{
    m_timer.stop();
    QPointer<CefWebPage> parentPage = m_page->pageFeatures.parentPage;
    if (!parentPage.isNull())
    {
    }
}

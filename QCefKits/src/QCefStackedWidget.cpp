/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "include/QCefStackedWidget.h"
#include "include/QCefWidget.h"
#include "include/QCefBrowserBar.h"
#include "src/CefWebPage.h"
#include "QCefKits_internal.h"
#include <QAbstractButton>
#include <QTabBar>
#include <QVariant>
#include <QLineEdit>
#include <QDebug>
#include <QThread>

QCefStackedWidget::QCefStackedWidget(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QStackedLayout();
    setLayout(m_layout);
}

QCefStackedWidget::~QCefStackedWidget()
{
//    qDebug() << "QCefStackedWidget::~QCefStackedWidget";
}

int QCefStackedWidget::count() const
{
    return m_layout->count();
}

QCefWidget *QCefStackedWidget::cefWidget(int index) const
{
    return qobject_cast<QCefWidget *>(m_layout->widget(index));
}

QCefWidget *QCefStackedWidget::createNewCefWidget(
        const QUrl &initUrl, int index)
{
    QCefWidget *w = new QCefWidget(this);
    w->init(initUrl);
    if (index >= 0)
    {
        m_layout->insertWidget(index, w);
        if (m_tabbar)
        {
            int newIndex = m_tabbar->insertTab(index, tr("New tab"));
            m_tabbar->setTabData(newIndex, QVariant::fromValue<QCefWidget *>(w));
//            qDebug() << "QCefStackedWidget::createNewCefWidget"
//                     << QVariant::fromValue<QCefWidget *>(w)
//                     << m_tabbar->tabData(index).value<QCefWidget *>()
//                     << newIndex << index;
        }
    }
    else
    {
        m_layout->addWidget(w);
        if (m_tabbar)
        {
            int newIndex = m_tabbar->addTab(tr("New tab"));
            m_tabbar->setTabData(newIndex, QVariant::fromValue<QCefWidget *>(w));
//            qDebug() << "QCefStackedWidget::createNewCefWidget"
//                     << QVariant::fromValue<QCefWidget *>(w)
//                     << m_tabbar->tabData(newIndex).value<QCefWidget *>()
//                     << newIndex << index;
        }
    }
//    if (m_tabbar)
//    {
//        qDebug() << "QCefStackedWidget::createNewCefWidget"
//                 << QVariant::fromValue<QCefWidget *>(w)
//                 << m_tabbar->tabData(index).value<QCefWidget *>();
//    }
    initConnections(w);
    return w;
}

QCefWidget *QCefStackedWidget::currentWidget()
{
    return qobject_cast<QCefWidget*>(m_layout->currentWidget());
}

QCefWidget *QCefStackedWidget::findUrl(const QUrl &url) const
{
    int i = 0;
    for (; i < m_layout->count(); i++)
    {
        QCefWidget *w = cefWidget(i);
        if (w)
        {
            if (w->url() == url)
            {
                return w;
            }
        }
    }
    return nullptr;
}

void QCefStackedWidget::setBrowserBar(QCefBrowserBar *bar)
{
    if (m_browserBar)
    {
        disconnect(m_browserBar, &QCefBrowserBar::goBack,
                   this, &QCefStackedWidget::goBack);
        disconnect(m_browserBar, &QCefBrowserBar::goForward,
                   this, &QCefStackedWidget::goForward);
        disconnect(m_browserBar, &QCefBrowserBar::reload,
                   this, &QCefStackedWidget::reload);
        disconnect(m_browserBar, &QCefBrowserBar::stopLoad,
                   this, &QCefStackedWidget::stopLoad);
        disconnect(m_browserBar, &QCefBrowserBar::navigate,
                   this, &QCefStackedWidget::navigate);
    }
    m_browserBar = bar;
    if (m_browserBar)
    {
        connect(m_browserBar, &QCefBrowserBar::goBack,
                this, &QCefStackedWidget::goBack);
        connect(m_browserBar, &QCefBrowserBar::goForward,
                this, &QCefStackedWidget::goForward);
        connect(m_browserBar, &QCefBrowserBar::reload,
                this, &QCefStackedWidget::reload);
        connect(m_browserBar, &QCefBrowserBar::stopLoad,
                this, &QCefStackedWidget::stopLoad);
        connect(m_browserBar, &QCefBrowserBar::navigate,
                this, &QCefStackedWidget::navigate);
    }
}

void QCefStackedWidget::setStackingMode(QStackedLayout::StackingMode stackingMode)
{
    m_layout->setStackingMode(stackingMode);
}

QStackedLayout::StackingMode QCefStackedWidget::stackingMode() const
{
    return m_layout->stackingMode();
}

void QCefStackedWidget::setTabbar(QTabBar *tabbar)
{
    if (m_tabbar)
    {
        disconnect(m_tabbar, &QTabBar::currentChanged,
                   this, &QCefStackedWidget::on_tabbar_currentChanged);
        disconnect(m_tabbar, &QTabBar::tabCloseRequested,
                   this, &QCefStackedWidget::on_tabbar_tabCloseRequested);
    }
    m_tabbar = tabbar;
    if (m_tabbar)
    {
        connect(m_tabbar, &QTabBar::currentChanged,
                this, &QCefStackedWidget::on_tabbar_currentChanged);
        connect(m_tabbar, &QTabBar::tabCloseRequested,
                this, &QCefStackedWidget::on_tabbar_tabCloseRequested);
    }
}

void QCefStackedWidget::goBack()
{
    QCefWidget *w = currentWidget();
    if (w)
    {
        w->goBack();
    }
}

void QCefStackedWidget::goForward()
{
    QCefWidget *w = currentWidget();
    if (w)
    {
        w->goForward();
    }
}

void QCefStackedWidget::stopLoad()
{
    QCefWidget *w = currentWidget();
    if (w)
    {
        w->stopLoad();
    }
}

void QCefStackedWidget::reload()
{
    QCefWidget *w = currentWidget();
    if (w)
    {
        w->reload();
    }
}

void QCefStackedWidget::navigate(QUrl url)
{
    QCefWidget *w = currentWidget();
    if (w)
    {
        w->loadUrl(url);
    }
}

void QCefStackedWidget::on_tabbar_currentChanged(int index)
{
    QTabBar *tab = qobject_cast<QTabBar *>(sender());
    QCefWidget *w = tab->tabData(index).value<QCefWidget *>();
//    qDebug() << "QCefStackedWidget::on_tabbar_currentChanged" << index << w
//             << tab->tabData(index)
//             << m_layout->count();
    if (w)
    {
        //m_layout->setCurrentWidget(w);
        m_layout->setCurrentIndex(index);
//        qDebug() << "QCefStackedWidget::on_tabbar_currentChanged" << m_layout->currentWidget();
        if (m_browserBar)
        {
            if (m_browserBar->addressBar())
            {
                m_browserBar->addressBar()->setText(w->url().toString());
            }
            if (m_browserBar->goBackwardButton())
            {
                m_browserBar->goBackwardButton()->setEnabled(w->canGoBack());
            }
            if (m_browserBar->goForwardButton())
            {
                m_browserBar->goForwardButton()->setEnabled(w->canGoForward());
            }
            if (m_browserBar->stopButton())
            {
                m_browserBar->stopButton()->setEnabled(w->isLoading());
            }
        }
    }
}

void QCefStackedWidget::on_tabbar_tabCloseRequested(int index)
{
    QTabBar *tab = qobject_cast<QTabBar *>(sender());
    QCefWidget *w = tab->tabData(index).value<QCefWidget *>();
    if (w)
    {
        qDebug() << "QCefStackedWidget::on_tabbar_tabCloseRequested" << w;
        w->closeBrowser();
    }
}

void QCefStackedWidget::on_cefWidget_browserCreated()
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit browserCreated(w);
}

void QCefStackedWidget::on_cefWidget_browserClosed()
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
//    qDebug() << "QCefStackedWidget::on_cefWidget_browserClosed" << w;
    if (m_tabbar)
    {
        int index = findTab(w);
        if ((index >= 0) && (index < m_tabbar->count()))
        {
            m_tabbar->removeTab(index);
        }
    }
    m_layout->removeWidget(w);
    emit browserClosed(w);
    w->deleteLater();
}

void QCefStackedWidget::on_cefWidget_consoleMessage(
        QCefKits_logLevel level,
        QString message,
        QString source,
        int line)
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit consoleMessage(w, level, message, source, line);
}

void QCefStackedWidget::on_cefWidget_loadStarted()
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit loadStarted(w);
}

void QCefStackedWidget::on_cefWidget_loadComplete()
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit loadComplete(w);
}

void QCefStackedWidget::on_cefWidget_loadFailed(
        int errorCode,
        QString errorText,
        QUrl failedUrl)
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit loadFailed(w, errorCode, errorText, failedUrl);
}

void QCefStackedWidget::on_cefWidget_loadingStateChanged(
        bool isLoading,
        bool canGoBack,
        bool canGoForward)
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit loadingStateChanged(w, isLoading, canGoBack, canGoForward);
}

void QCefStackedWidget::on_cefWidget_newBrowserCreated(
        QCefWidget *newPage,
        int x,
        bool xSet,
        int y,
        bool ySet,
        int width,
        bool widthSet,
        int height,
        bool heightSet,
        bool menuBarVisible,
        bool statusBarVisible,
        bool toolBarVisible,
        bool scrollbarsVisible,
        bool isPopup)
{
//    qDebug() << "QCefStackedWidget::on_cefWidget_newBrowserCreated"
//             << newPage << isPopup;
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit newBrowserCreated(w, newPage, x, xSet, y, ySet, width, widthSet, height, heightSet,
                           menuBarVisible, statusBarVisible, toolBarVisible, scrollbarsVisible, isPopup);
    if (!isPopup)
    {
//        qDebug() << "QCefStackedWidget::on_cefWidget_newBrowserCreated"
//                 << "before set parent"
//                 << thread() << newPage->thread() << QThread::currentThread();
        newPage->setParent(this);
//        qDebug() << "QCefStackedWidget::on_cefWidget_newBrowserCreated"
//                 << "after set parent";
        int index = m_layout->addWidget(newPage);
        int newIndex = m_tabbar->addTab(tr("New tab"));
        m_tabbar->setTabData(newIndex, QVariant::fromValue<QCefWidget *>(newPage));
//        qDebug() << "QCefStackedWidget::on_cefWidget_newBrowserCreated"
//                 << QVariant::fromValue<QCefWidget *>(newPage)
//                 << m_tabbar->tabData(newIndex).value<QCefWidget *>()
//                 << newIndex << index;
        initConnections(newPage);
    }
    else
    {
//        qDebug() << "QCefStackedWidget::on_cefWidget_newBrowserCreated"
//                 << "before show"
//                 << thread() << newPage->thread() << QThread::currentThread();
        newPage->show();
//        qDebug() << "QCefStackedWidget::on_cefWidget_newBrowserCreated"
//                 << "after show";
        initConnections(newPage);
    }
}

void QCefStackedWidget::on_cefWidget_titleChanged(QString title)
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit titleChanged(w, title);
    if (m_tabbar)
    {
        int index = findTab(w);
        if (index >= 0)
        {
            m_tabbar->setTabText(index, title);
        }
        else
        {
            qDebug() << "QCefStackedWidget::on_cefWidget_titleChanged: warning: tab of"
                     << w << "not found";
        }
    }
}

void QCefStackedWidget::on_cefWidget_urlChanged(QUrl url)
{
    QCefWidget *w = qobject_cast<QCefWidget *>(sender());
    emit urlChanged(w, url);
    if (m_browserBar)
    {
        QLineEdit *bar = m_browserBar->addressBar();
        if (bar)
        {
            bar->setText(url.toString());
        }
    }
}

int QCefStackedWidget::findTab(QCefWidget *w)
{
    if (m_tabbar.isNull())
    {
        return -1;
    }
    int i = 0;
    for (; i < m_tabbar->count(); i++)
    {
        QCefWidget *tw = m_tabbar->tabData(i).value<QCefWidget *>();
        if (tw == w)
        {
            return i;
        }
    }
    return -1;
}

void QCefStackedWidget::initConnections(QCefWidget *w)
{
    connect(w, &QCefWidget::browserCreated,
            this, &QCefStackedWidget::on_cefWidget_browserCreated);
    connect(w, &QCefWidget::browserClosed,
            this, &QCefStackedWidget::on_cefWidget_browserClosed);
    connect(w, &QCefWidget::consoleMessage,
            this, &QCefStackedWidget::on_cefWidget_consoleMessage);
    connect(w, &QCefWidget::loadStarted,
            this, &QCefStackedWidget::on_cefWidget_loadStarted);
    connect(w, &QCefWidget::loadComplete,
            this, &QCefStackedWidget::on_cefWidget_loadComplete);
    connect(w, &QCefWidget::loadFailed,
            this, &QCefStackedWidget::on_cefWidget_loadFailed);
    connect(w, &QCefWidget::loadingStateChanged,
            this, &QCefStackedWidget::on_cefWidget_loadingStateChanged);
    connect(w, &QCefWidget::newBrowserCreated,
            this, &QCefStackedWidget::on_cefWidget_newBrowserCreated,
            Qt::QueuedConnection);
    connect(w, &QCefWidget::titleChanged,
            this, &QCefStackedWidget::on_cefWidget_titleChanged);
    connect(w, &QCefWidget::urlChanged,
            this, &QCefStackedWidget::on_cefWidget_urlChanged);
}

/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef QCEFWIDGET_H
#define QCEFWIDGET_H

#include "QCefKitsGlobal.h"
#include <QWidget>
#include <QTimer>
#include <QUrl>
#include <QSharedPointer>

class CefWebPage;

class QCEFKITS_EXPORT QCefWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QCefWidget(QWidget *parent = nullptr);
    ~QCefWidget();

    void init(const QUrl &url);
    void setWebPage(QSharedPointer<CefWebPage> page);
    void loadUrl(const QUrl &url);
    void closeBrowser();

Q_SIGNALS:
    void consoleMessage(QCefKits_logLevel level,
                        const QString& message,
                        const QString& source,
                        int line);
    void urlChanged(QUrl url);
    void titleChanged(QString title);
    void browserCreated();
    void browserClosed();
    void loadStarted();
    void loadComplete();
    void loadFailed();
    void newBrowserCreated(QCefWidget *newPage,
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
                           bool isPopup);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void onBrowserCreated();
    void onTimerTimeout();
//    void onBrowserClosed(int id);
//    void onLoadStarted();
//    void onLoadComplete();
//    void onLoadFailed();

private:
    QSharedPointer<CefWebPage> m_page;
    QTimer m_timer;
};

#endif // QCEFWIDGET_H

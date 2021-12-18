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
    Q_PROPERTY(QUrl url READ url WRITE loadUrl NOTIFY urlChanged)

public:
    explicit QCefWidget(QWidget *parent = nullptr);
    ~QCefWidget();

    void closeBrowser();
    void executeJs(const QString &code,
                   const QUrl &scriptUrl = QUrl(),
                   int startLine = 0);
    void init(const QUrl &url);
    bool hasCefBrowser();
    void loadUrl(const QUrl &url);
    void setBrowserFocus(bool f);
    void setWebPage(QSharedPointer<CefWebPage> page);
    QUrl url() const;

public Q_SLOTS:
    void goBack();
    void goForward();
    void reload();
    void stopLoad();  //stop loading
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void doDelete();
    void selectAll();
    bool canGoBack();
    bool canGoForward();
    bool isLoading();

Q_SIGNALS:
    void browserCreated();
    void browserClosed();
    void consoleMessage(QCefKits_logLevel level,
                        QString message,
                        QString source,
                        int line);
    void loadStarted();
    void loadComplete();
    void loadFailed(int errorCode,
                    QString errorText,
                    QUrl failedUrl);
    void loadingStateChanged(bool isLoading,
                             bool canGoBack,
                             bool canGoForward);
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
    void titleChanged(QString title);
    void urlChanged(QUrl url);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private Q_SLOTS:
    void onBrowserCreated();
    void onBrowserClosed();
    void onTimerTimeout();
    void onNewBrowserRequest(QSharedPointer<CefWebPage> page);
    void onUrlChanged(QUrl url);
//    void onLoadStarted();
//    void onLoadComplete();
//    void onLoadFailed();

private:
    QSharedPointer<CefWebPage> m_page;
    QTimer m_timer;
    QUrl m_currentUrl;
};

#endif // QCEFWIDGET_H

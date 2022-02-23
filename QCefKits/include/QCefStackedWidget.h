/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef QCEFSTACKEDWIDGET_H
#define QCEFSTACKEDWIDGET_H

#include "QCefKitsGlobal.h"
#include <QWidget>
#include <QPointer>
#include <QUrl>
#include <QStackedLayout>

class QCefWidget;
class QTabBar;
class QCefBrowserBar;

class QCEFKITS_EXPORT QCefStackedWidget : public QWidget
{
    Q_OBJECT
    //Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(QStackedLayout::StackingMode stackingMode READ stackingMode WRITE setStackingMode)
    Q_PROPERTY(int count READ count)

public:
    explicit QCefStackedWidget(QWidget *parent = nullptr);
    ~QCefStackedWidget();

    int count() const;
    QCefWidget *cefWidget(int index) const;
    QCefWidget *createNewCefWidget(const QUrl &initUrl, int index = -1);
    QCefWidget *currentWidget();
    QCefWidget *findUrl(const QUrl &url) const;
    void setBrowserBar(QCefBrowserBar *bar);
    void setStackingMode(QStackedLayout::StackingMode stackingMode);
    QStackedLayout::StackingMode stackingMode() const;
    void setTabbar(QTabBar *tabbar);

public Q_SLOTS:
    void goBack();
    void goForward();
    void stopLoad();
    void reload();
    void navigate(QUrl url);

Q_SIGNALS:
    void browserCreated(QCefWidget *w);
    void browserClosed(QCefWidget *w);
    void consoleMessage(QCefWidget *w,
                        QCefKits_logLevel level,
                        const QString& message,
                        const QString& source,
                        int line);
    void loadStarted(QCefWidget *w);
    void loadComplete(QCefWidget *w);
    void loadFailed(QCefWidget *w,
                    int errorCode,
                    QString errorText,
                    QUrl failedUrl);
    void loadingStateChanged(QCefWidget *w,
                             bool isLoading,
                             bool canGoBack,
                             bool canGoForward);
    void newBrowserCreated(QCefWidget *w,
                           QCefWidget *newPage,
                           int x, bool xSet,
                           int y, bool ySet,
                           int width, bool widthSet,
                           int height, bool heightSet,
                           bool menuBarVisible,
                           bool statusBarVisible,
                           bool toolBarVisible,
                           bool scrollbarsVisible,
                           bool isPopup);
    void titleChanged(QCefWidget *w, QString title);
    void urlChanged(QCefWidget *w, QUrl url);

protected Q_SLOTS:
    virtual void on_tabbar_currentChanged(int index);
    virtual void on_tabbar_tabCloseRequested(int index);
    virtual void on_cefWidget_browserCreated();
    virtual void on_cefWidget_browserClosed();
    virtual void on_cefWidget_consoleMessage(
            QCefKits_logLevel level,
            QString message,
            QString source,
            int line);
    virtual void on_cefWidget_loadStarted();
    virtual void on_cefWidget_loadComplete();
    virtual void on_cefWidget_loadFailed(
            int errorCode,
            QString errorText,
            QUrl failedUrl);
    virtual void on_cefWidget_loadingStateChanged(
            bool isLoading,
            bool canGoBack,
            bool canGoForward);
    virtual void on_cefWidget_newBrowserCreated(
            QCefWidget *newPage,
            int x, bool xSet,
            int y, bool ySet,
            int width, bool widthSet,
            int height, bool heightSet,
            bool menuBarVisible,
            bool statusBarVisible,
            bool toolBarVisible,
            bool scrollbarsVisible,
            bool isPopup);
    virtual void on_cefWidget_titleChanged(QString title);
    virtual void on_cefWidget_urlChanged(QUrl url);

protected:
    int findTab(QCefWidget *w);
    virtual void initConnections(QCefWidget *w);

    QPointer<QTabBar> m_tabbar;
    QPointer<QCefBrowserBar> m_browserBar;
    QStackedLayout *m_layout;
};

#endif // QCEFSTACKEDWIDGET_H

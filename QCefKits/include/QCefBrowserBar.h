#ifndef QCEFBROWSERBAR_H
#define QCEFBROWSERBAR_H

#include "QCefKitsGlobal.h"
#include <QWidget>
#include <QUrl>

namespace Ui {
class QCefBrowserBar;
}

class QAbstractButton;
class QLineEdit;

class QCEFKITS_EXPORT QCefBrowserBar : public QWidget
{
    Q_OBJECT

public:
    explicit QCefBrowserBar(QWidget *parent = nullptr);
    ~QCefBrowserBar();

    void init();
    virtual QAbstractButton *goBackwardButton();
    virtual QAbstractButton *goForwardButton();
    virtual QAbstractButton *reloadButton();
    virtual QAbstractButton *stopButton();
    virtual QAbstractButton *navigateButton();
    virtual QLineEdit *addressBar();

Q_SIGNALS:
    void goBack();
    void goForward();
    void reload();
    void stopLoad();
    void navigate(QUrl url);

private Q_SLOTS:
    void onNavigate();

private:
    Ui::QCefBrowserBar *ui;
};

#endif // QCEFBROWSERBAR_H

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QCefWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    Widget(QCefWidget *browser, QWidget *parent = nullptr);
    ~Widget();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pushButton_clicked();
    void on_lineEdit_returnPressed();

private:
    void connectBrowserSignals();

    Ui::Widget *ui;
    QCefWidget *m_browser;
};
#endif // WIDGET_H

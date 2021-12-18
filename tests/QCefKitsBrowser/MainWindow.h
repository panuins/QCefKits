#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCefStackedWidget.h>
#include <QCefBrowserBar.h>
#include <QMainWindow>
#include <QTabBar>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVBoxLayout *m_layout;
    QTabBar *m_tabbar;
    QCefStackedWidget *m_stackedWidget;
    QCefBrowserBar *m_browserBar;
};
#endif // MAINWINDOW_H

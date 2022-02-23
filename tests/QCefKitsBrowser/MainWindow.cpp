#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QApplication>
#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_tabbar = new QTabBar(ui->centralwidget);
    m_tabbar->setExpanding(false);
    m_tabbar->setTabsClosable(true);
    m_browserBar = new QCefBrowserBar(ui->centralwidget);
    m_stackedWidget = new QCefStackedWidget(ui->centralwidget);
    m_stackedWidget->setTabbar(m_tabbar);
    m_stackedWidget->setBrowserBar(m_browserBar);
    m_layout = new QVBoxLayout(ui->centralwidget);
    m_layout->addWidget(m_tabbar);
    m_layout->addWidget(m_browserBar);
    m_layout->addWidget(m_stackedWidget);
    m_stackedWidget->createNewCefWidget(QUrl("https://gitee.com/panuins/test-html/raw/main/html/index.html"));
    m_stackedWidget->createNewCefWidget(QUrl("http://192.168.56.1"));
    connect(qApp, &QGuiApplication::lastWindowClosed,
            this, [](){
        qDebug() << "QGuiApplication::lastWindowClosed";
    });
    connect(qApp, &QCoreApplication::aboutToQuit,
            this, [](){
        qDebug() << "QGuiApplication::aboutToQuit";
    });
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow";
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow::closeEvent";
    QMainWindow::closeEvent(event);
}


void MainWindow::on_actionNew_window_triggered()
{
    MainWindow *window = new MainWindow;
    window->show();
}

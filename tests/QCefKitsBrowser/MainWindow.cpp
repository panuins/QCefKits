#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QUrl>

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
    m_stackedWidget->createNewCefWidget(QUrl("http://192.168.222.1"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


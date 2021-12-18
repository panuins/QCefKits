#include "include/QCefBrowserBar.h"
#include "ui_QCefBrowserBar.h"
#include <QApplication>
#include <QProxyStyle>
#include <QUrl>

QCefBrowserBar::QCefBrowserBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QCefBrowserBar)
{
    init();
}

QCefBrowserBar::~QCefBrowserBar()
{
    delete ui;
}

void QCefBrowserBar::init()
{
    ui->setupUi(this);
    if (qApp)
    {
        QStyle *style = this->style();
        if (!style)
        {
            style = qApp->style();
        }
        if (!style)
        {
            style = new QProxyStyle;
        }
        ui->toolButton_goBackward->setIcon(style->standardIcon(QStyle::SP_ArrowBack));
        ui->toolButton_goForward->setIcon(style->standardIcon(QStyle::SP_ArrowForward));
        ui->toolButton_reload->setIcon(style->standardIcon(QStyle::SP_BrowserReload));
        ui->toolButton_stop->setIcon(style->standardIcon(QStyle::SP_BrowserStop));
    }
    connect(ui->toolButton_goBackward, SIGNAL(clicked()),
            this, SIGNAL(goBack()));
    connect(ui->toolButton_goForward, SIGNAL(clicked()),
            this, SIGNAL(goForward()));
    connect(ui->toolButton_reload, SIGNAL(clicked()),
            this, SIGNAL(reload()));
    connect(ui->toolButton_stop, SIGNAL(clicked()),
            this, SIGNAL(stopLoad()));
    connect(ui->lineEdit_url, &QLineEdit::returnPressed,
            this, &QCefBrowserBar::onNavigate);
    connect(ui->pushButton_go, &QPushButton::clicked,
            this, &QCefBrowserBar::onNavigate);
}

QAbstractButton *QCefBrowserBar::goBackwardButton()
{
    return ui->toolButton_goBackward;
}

QAbstractButton *QCefBrowserBar::goForwardButton()
{
    return ui->toolButton_goForward;
}

QAbstractButton *QCefBrowserBar::reloadButton()
{
    return ui->toolButton_reload;
}

QAbstractButton *QCefBrowserBar::stopButton()
{
    return ui->toolButton_stop;
}

QAbstractButton *QCefBrowserBar::navigateButton()
{
    return ui->pushButton_go;
}

QLineEdit *QCefBrowserBar::addressBar()
{
    return ui->lineEdit_url;
}

void QCefBrowserBar::onNavigate()
{
    if (!ui->lineEdit_url->text().contains(":"))
    {
        ui->lineEdit_url->setText(QString("http://")+ui->lineEdit_url->text());
    }
    emit navigate(ui->lineEdit_url->text());
}

#include "JSDialogHandler.h"
#include "QCefKits_internal.h"
#include <include/cef_parser.h>
#include <QDebug>
#include <QThread>

namespace QCefKits
{

JSDialogHandler::JSDialogHandler()
{
}

int JSDialogHandler::execPendingDialog()
{
    bool ret = initJsDialog(m_dialogInfo.origin_url,
                            m_dialogInfo.dialog_type,
                            m_dialogInfo.message_text,
                            m_dialogInfo.default_prompt_text);
    if (!ret)
    {
        qDebug() << "JSDialogHandler::execPendingDialog: error: init dialog failed";
        if (m_messagebox)
        {
            delete m_messagebox.data();
        }
        if (m_inputDialog)
        {
            delete m_inputDialog.data();
        }
        return -1/*true*/;
    }
    if ((!m_messagebox) && (!m_inputDialog))
    {
        qDebug() << "JSDialogHandler::execPendingDialog: error: (!m_messagebox) && (!m_inputDialog)";
        return -1/*true*/;
    }
    if (m_messagebox)
    {
        m_ret = m_messagebox->exec();
    }
    if (m_inputDialog)
    {
        m_ret = m_inputDialog->exec();
    }
    m_execed = true;
    return m_ret;
}

bool JSDialogHandler::initJsDialog(const CefString& origin_url,
                                   JSDialogType dialog_type,
                                   const CefString& message_text,
                                   const CefString& default_prompt_text)
{
    QString text = QString::fromStdString(message_text.ToString());
    QString title = QString::fromStdString(
          CefFormatUrlForSecurityDisplay(origin_url).ToString());
    {
        switch (dialog_type)
        {
          case JSDIALOGTYPE_ALERT:
        {
            m_messagebox = new QMessageBox();
//            m_messagebox->setWindowFlags(/*windowFlags() | */ Qt::WindowStaysOnTopHint);
//            m_messagebox->setAttribute(Qt::WA_AlwaysStackOnTop);
            m_messagebox->setWindowTitle(title);
            m_messagebox->setText(text);
            m_messagebox->addButton(QMessageBox::Ok);
            break;
          }
          case JSDIALOGTYPE_CONFIRM:
        {
            m_messagebox = new QMessageBox();
//            m_messagebox->setWindowFlags(/*windowFlags() | */ Qt::WindowStaysOnTopHint);
//            m_messagebox->setAttribute(Qt::WA_AlwaysStackOnTop);
            m_messagebox->setWindowTitle(title);
            m_messagebox->setText(text);
            m_messagebox->addButton(QMessageBox::Yes);
            m_messagebox->addButton(QMessageBox::No);
            break;
          }
          case JSDIALOGTYPE_PROMPT:
        {
            m_inputDialog = new QInputDialog();
//            dialog->setWindowFlags(/*windowFlags() | */ Qt::WindowStaysOnTopHint);
//            dialog->setAttribute(Qt::WA_AlwaysStackOnTop);
            m_inputDialog->setWindowTitle(title);
            m_inputDialog->setLabelText(text);
            QString default_value =
                QString::fromStdString(default_prompt_text.ToString());
            m_inputDialog->setTextValue(default_value);
            break;
          }
        }
    }
    return true;
}

bool JSDialogHandler::OnJSDialog(CefRefPtr<CefBrowser> /*browser*/,
                                 const CefString& origin_url,
                                 JSDialogType dialog_type,
                                 const CefString& message_text,
                                 const CefString& default_prompt_text,
                                 CefRefPtr<CefJSDialogCallback> callback,
                                 bool& /*suppress_message*/)
{
    CEF_REQUIRE_UI_THREAD();
    m_dialogInfo.origin_url = origin_url;
    m_dialogInfo.dialog_type = dialog_type;
    m_dialogInfo.message_text = message_text;
    m_dialogInfo.default_prompt_text = default_prompt_text;
    if (QCefKits::g_cefSettings.multi_threaded_message_loop)
    {
        m_execed = false;
        pushPendingDialog(this);
        while (!m_execed)
        {
            QThread::msleep(10);
        }
    }
    else
    {
        execPendingDialog();
    }
    if (m_messagebox)
    {
        qDebug() << m_ret;
        callback->Continue((static_cast<QMessageBox::StandardButton>(m_ret) == QMessageBox::Yes)
                           || (static_cast<QMessageBox::StandardButton>(m_ret) == QMessageBox::Ok),
                           CefString());
        delete m_messagebox.data();
    }
    else if (m_inputDialog)
    {
        callback->Continue(m_ret == QDialog::Accepted,
                           m_inputDialog->textValue().toStdWString());
        delete m_inputDialog.data();
    }
    return true;
}

bool JSDialogHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                           const CefString& message_text,
                                           bool is_reload,
                                           CefRefPtr<CefJSDialogCallback> callback)
{
    std::wstring new_message_text = message_text.ToWString();
    if (is_reload)
    {
        new_message_text = new_message_text + QObject::tr("\n\nIs it OK to reload this page?").toStdWString();
    }
    else
    {
        new_message_text = new_message_text + QObject::tr("\n\nIs it OK to leave this page?").toStdWString();
    }
    bool suppress_message = false;

    return OnJSDialog(browser, CefString(), JSDIALOGTYPE_CONFIRM,
                      new_message_text, CefString(), callback, suppress_message);
}

void JSDialogHandler::OnResetDialogState(CefRefPtr<CefBrowser> /*browser*/)
{
    if (m_inputDialog)
    {
        m_inputDialog->done(-1);
    }
    if (m_messagebox)
    {
        m_messagebox->done(-1);
    }
}

void JSDialogHandler::OnDialogClosed(CefRefPtr<CefBrowser> /*browser*/)
{
    if (m_inputDialog)
    {
        m_inputDialog->done(-1);
    }
    if (m_messagebox)
    {
        m_messagebox->done(-1);
    }
}
}

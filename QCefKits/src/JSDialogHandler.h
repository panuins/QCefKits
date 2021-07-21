#ifndef JSDIALOGHANDLER_H
#define JSDIALOGHANDLER_H
#include <QMessageBox>
#include <QInputDialog>
#include <QPointer>
#include <include/cef_client.h>
#include <include/wrapper/cef_helpers.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#include "IPendingDialog.h"

namespace QCefKits
{

class JSDialogHandler : public CefJSDialogHandler, public IPendingDialog
{
public:
    struct DialogInfo
    {
        CefString origin_url;
        JSDialogType dialog_type;
        CefString message_text;
        CefString default_prompt_text;
    };
    JSDialogHandler();

    virtual int execPendingDialog() OVERRIDE;
    virtual bool initJsDialog(const CefString& origin_url,
                              JSDialogType dialog_type,
                              const CefString& message_text,
                              const CefString& default_prompt_text);

    virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                            const CefString& origin_url,
                            JSDialogType dialog_type,
                            const CefString& message_text,
                            const CefString& default_prompt_text,
                            CefRefPtr<CefJSDialogCallback> callback,
                            bool& suppress_message) OVERRIDE;
    virtual bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                      const CefString& message_text,
                                      bool is_reload,
                                      CefRefPtr<CefJSDialogCallback> callback) OVERRIDE;
    virtual void OnResetDialogState(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnDialogClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;

protected:
    struct DialogInfo m_dialogInfo;
    QPointer<QMessageBox> m_messagebox;
    QPointer<QInputDialog> m_inputDialog;
    volatile int m_ret = 0;
    volatile bool m_execed = false;

    IMPLEMENT_REFCOUNTING(JSDialogHandler);
    DISALLOW_COPY_AND_ASSIGN(JSDialogHandler);
};
}

#endif // JSDIALOGHANDLER_H

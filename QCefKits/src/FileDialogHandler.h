/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef FILEDIALOGHANDLER_H
#define FILEDIALOGHANDLER_H
#include <QFileDialog>
#include <QPointer>
#include <include/cef_client.h>
#include <include/wrapper/cef_helpers.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#include "IPendingDialog.h"

namespace QCefKits
{

class FileDialogHandler : public CefDialogHandler, public IPendingDialog
{
public:
    struct DialogInfo
    {
        FileDialogMode mode;
        CefString title;
        CefString default_file_path;
        std::vector<CefString> accept_filters;
        int selected_accept_filter;
    };

    FileDialogHandler();

    virtual int execPendingDialog() override;
    virtual bool initFileDialog(FileDialogMode mode,
                                const CefString& title,
                                const CefString& default_file_path,
                                const std::vector<CefString>& accept_filters,
                                int selected_accept_filter);
    bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                      FileDialogMode mode,
                      const CefString& title,
                      const CefString& default_file_path,
                      const std::vector<CefString>& accept_filters,
                      int selected_accept_filter,
                      CefRefPtr<CefFileDialogCallback> callback) override;

protected:
    struct DialogInfo m_dialogInfo;
    QPointer<QFileDialog> m_fileDialog;
    volatile int m_ret = 0;
    volatile bool m_execed = false;

    IMPLEMENT_REFCOUNTING(FileDialogHandler);
    DISALLOW_COPY_AND_ASSIGN(FileDialogHandler);
};
}
#endif // FILEDIALOGHANDLER_H

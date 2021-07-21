/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#include "FileDialogHandler.h"
#include "QCefKits_internal.h"
#include <include/cef_parser.h>
#include <QDir>
#include <QMimeDatabase>
#include <QThread>

static QString formatCefNameFilterString(const QString &exts)
{
    QString ret;
    QStringList extensions = exts.split(';', QString::SkipEmptyParts);
    for (QString ext : extensions)
    {
        if (!ext.isEmpty() && ext.startsWith('.'))
        {
            if (!ret.isEmpty())
            {
                ret.append(" ");
            }
            ret.append("*"+ext);
        }
    }
    return ret;
}

//"text/*"
//"image/*"
//"image/.png;.gif;.jpg"
//"image/jpeg"
//".txt"
//"Image Types|.png;.gif;.jpg"
static QString cefFilterToQtNameFilter(const CefString &filter)
{
    QString filt = QString::fromStdWString(filter.ToWString());
    if (filt.isEmpty())
    {
        return QObject::tr("Any files (*)");
    }

    int posSep = filt.indexOf('|');
    if (posSep >= 0)
    {
        QString extensionStr;
        QString description;
        description = filt.left(posSep);

        QString exts = filt.mid(posSep + 1);
        return QObject::tr("%1 (%2)").arg(description).arg(formatCefNameFilterString(exts));
    }
    else if (filt.startsWith('.'))
    {
        QString ext = filt.mid(1);
        QMimeDatabase db;
        QList<QMimeType> mimeTypes = db.mimeTypesForFileName("a"+filt);
        if (!mimeTypes.isEmpty())
        {
            QString description = mimeTypes.first().comment();
            description[0] = description[0].toUpper();
            return QObject::tr("%1 (*.%2)").arg(description).arg(ext);
        }
        return QObject::tr("%1 files (*.%1)").arg(ext);
    }

    QMimeDatabase db;
    QString description;
    int posSlash = filt.indexOf('/');
    QString mimeStr = filt.left(posSlash);
    if (mimeStr == "audio")
    {
        description = QObject::tr("Audio Files");
    }
    else if (mimeStr == "image")
    {
        description = QObject::tr("Image Files");
    }
    else if (mimeStr == "text")
    {
        description = QObject::tr("Text Files");
    }
    else if (mimeStr == "video")
    {
        description = QObject::tr("video Files");
    }
    else if (!mimeStr.isEmpty())
    {
        description = mimeStr+QObject::tr(" files");
    }
    else
    {
        description = QObject::tr("*");
    }
    if (filt.endsWith("/*"))
    {
        QStringList patterns;
        for (QMimeType mimeType : db.allMimeTypes())
        {
            if (mimeType.name().startsWith(mimeStr))
            {
                patterns += mimeType.globPatterns();
            }
//            qDebug() << "-------" << mimeType.name() << mimeType.comment() << mimeType.globPatterns() << mimeType;
        }
        return QObject::tr("%1 (%2)").arg(description).arg(patterns.join(' '));
    }
    else
    {
        int posSlash = filt.indexOf('/');
        QMimeType mimeType(db.mimeTypeForName(filt.left(posSlash)));
        if (mimeType.isValid())
        {
            if (mimeType.isDefault())
            {
                return QObject::tr("All files (*)");
            }
            else
            {
                QString patterns = mimeType.globPatterns().join(' ');
                QString description = mimeType.comment();
                description[0] = description[0].toUpper();
                return QObject::tr("%1 (%2)").arg(description).arg(patterns);
            }
        }
        else
        {
            return QObject::tr("%1 (%2)").arg(description).arg(formatCefNameFilterString(filt.mid(posSlash+1)));
        }
    }
//    return QObject::tr("All files (*)");
}

static QStringList cefFiltersToQtNameFilters(const std::vector<CefString> &filters)
{
    QStringList ret;
    for (CefString filter : filters)
    {
        ret.append(cefFilterToQtNameFilter(filter));
    }
    return ret;
}

namespace QCefKits
{

FileDialogHandler::FileDialogHandler()
{

}

int FileDialogHandler::execPendingDialog()
{
    bool ret = initFileDialog(m_dialogInfo.mode,
                              m_dialogInfo.title,
                              m_dialogInfo.default_file_path,
                              m_dialogInfo.accept_filters,
                              m_dialogInfo.selected_accept_filter);
    if (!ret)
    {
        if (m_fileDialog)
        {
            delete m_fileDialog.data();
        }
//        callback->Cancel();
//        return true;
        return -1;
    }
    if (!m_fileDialog)
    {
//        callback->Cancel();
//        return true;
        return -1;
    }
    if (m_fileDialog)
    {
        m_ret = m_fileDialog->exec();
    }
    m_execed = true;
    return m_ret;
}

bool FileDialogHandler::initFileDialog(FileDialogMode mode,
                                       const CefString& title,
                                       const CefString& default_file_path,
                                       const std::vector<CefString>& accept_filters,
                                       int selected_accept_filter)
{
    std::vector<CefString> files;

    FileDialogMode mode_type =
        static_cast<FileDialogMode>(mode & FILE_DIALOG_TYPE_MASK);

    QString title_str;
    if (!title.empty())
    {
        title_str = QString::fromStdWString(title);
    }
    else
    {
        switch (mode_type)
        {
        case FILE_DIALOG_OPEN:
            title_str = QObject::tr("Open File");
            break;
        case FILE_DIALOG_OPEN_MULTIPLE:
            title_str = QObject::tr("Open Files");
            break;
        case FILE_DIALOG_OPEN_FOLDER:
            title_str = QObject::tr("Open Folder");
            break;
        case FILE_DIALOG_SAVE:
            title_str = QObject::tr("Save File");
            break;
        default:
            break;
        }
    }

    m_fileDialog = new QFileDialog;
    m_fileDialog->setWindowTitle(title_str);
    QDir::Filters fileFilter(QDir::AllEntries);
    switch (mode_type)
    {
    case FILE_DIALOG_OPEN:
        m_fileDialog->setFileMode(QFileDialog::ExistingFile);
        m_fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    {
        QStringList filters = cefFiltersToQtNameFilters(accept_filters);
        m_fileDialog->setNameFilters(filters);
        if ((selected_accept_filter >= 0) && (selected_accept_filter < filters.size()))
        {
            m_fileDialog->selectNameFilter(filters.at(selected_accept_filter));
        }
    }
        break;
    case FILE_DIALOG_OPEN_MULTIPLE:
        m_fileDialog->setFileMode(QFileDialog::ExistingFiles);
        m_fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    {
        QStringList filters = cefFiltersToQtNameFilters(accept_filters);
        m_fileDialog->setNameFilters(filters);
        if ((selected_accept_filter >= 0) && (selected_accept_filter < filters.size()))
        {
            m_fileDialog->selectNameFilter(filters.at(selected_accept_filter));
        }
    }
        break;
    case FILE_DIALOG_OPEN_FOLDER:
        m_fileDialog->setFileMode(QFileDialog::DirectoryOnly);
        m_fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        //on ? (*this |= f) : (*this &= ~Int(f));
        fileFilter &= ~QDir::Files;
//        fileFilter.setFlag(QDir::Files, false);
        break;
    case FILE_DIALOG_SAVE:
    {
        QStringList filters = cefFiltersToQtNameFilters(accept_filters);
        m_fileDialog->setNameFilters(filters);
        if ((selected_accept_filter >= 0) && (selected_accept_filter < filters.size()))
        {
            m_fileDialog->selectNameFilter(filters.at(selected_accept_filter));
        }
    }
        if (mode & FILE_DIALOG_HIDEREADONLY_FLAG)
        {
            fileFilter &= ~QDir::Readable;
            fileFilter &= ~QDir::Executable;
        }
        else
        {
            fileFilter |= QDir::Readable;
            fileFilter |= QDir::Executable;
        }
        m_fileDialog->setConfirmOverwrite(mode & FILE_DIALOG_OVERWRITEPROMPT_FLAG);
        m_fileDialog->setFileMode(QFileDialog::AnyFile);
        m_fileDialog->setAcceptMode(QFileDialog::AcceptSave);
        if (default_file_path.empty())
        {
            m_fileDialog->setDirectory(QDir::home());
        }
        else
        {
            m_fileDialog->setDirectory(QString::fromStdWString(default_file_path.ToWString()));
        }
        break;
    default:
        return false;
    }
    m_fileDialog->setFilter(fileFilter);
    m_execed = false;
    return true;
}

bool FileDialogHandler::OnFileDialog(CefRefPtr<CefBrowser> /*browser*/,
                                     FileDialogMode mode,
                                     const CefString& title,
                                     const CefString& default_file_path,
                                     const std::vector<CefString>& accept_filters,
                                     int selected_accept_filter,
                                     CefRefPtr<CefFileDialogCallback> callback)
{
    CEF_REQUIRE_UI_THREAD();
    m_dialogInfo.mode = mode;
    m_dialogInfo.title = title;
    m_dialogInfo.default_file_path = default_file_path;
    m_dialogInfo.accept_filters = accept_filters;
    m_dialogInfo.selected_accept_filter = selected_accept_filter;
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
    if (m_ret == QDialog::Accepted)
    {
        QStringList filters = m_fileDialog->nameFilters();
        QStringList files = m_fileDialog->selectedFiles();
        std::vector<CefString> selectedFiles;
        for (QString file : files)
        {
            selectedFiles.push_back(file.toStdWString());
        }
        callback->Continue(filters.indexOf(m_fileDialog->selectedNameFilter()), selectedFiles);
    }
    else
    {
        callback->Cancel();
    }
    if (m_fileDialog)
    {
        delete m_fileDialog.data();
    }
    return true;
}
}

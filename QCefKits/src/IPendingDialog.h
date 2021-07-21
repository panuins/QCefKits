#ifndef IPENDINGDIALOG_H
#define IPENDINGDIALOG_H

#include <include/cef_base.h>

namespace QCefKits
{

class IPendingDialog : public virtual CefBaseRefCounted
{
public:
    IPendingDialog();
    virtual ~IPendingDialog();
    virtual int execPendingDialog() = 0;
};
}

#endif // IPENDINGDIALOG_H

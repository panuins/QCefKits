#ifndef QCEFKITS_INTERNAL_H
#define QCEFKITS_INTERNAL_H
#include "include/QCefKits.h"
#include "IPendingDialog.h"
#include <include/cef_base.h>
#include <QTimer>
#include <queue>

namespace QCefKits
{
extern CefSettings g_cefSettings;
extern QTimer *g_timer;

void pushPendingDialog(CefRefPtr<IPendingDialog> dialog);
CefRefPtr<IPendingDialog> popPendingDialog();
}
#endif // QCEFKITS_INTERNAL_H

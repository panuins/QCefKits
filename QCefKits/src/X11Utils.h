#ifndef X11UTILS_H
#define X11UTILS_H

#include <include/cef_base.h>
#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace QCefKits
{
unsigned long CreateCefBrowserWindow(int width, int height);
void DestoryCefBrowserWindow(unsigned long winid);
int XErrorHandlerImpl(Display* display, XErrorEvent* event);
int XIOErrorHandlerImpl(Display* display);
void SetXErrorHandler();
void ReparentWindow(CefWindowHandle parent, CefWindowHandle child, CefWindowHandle container);
Qt::KeyboardModifiers NativeToQtKeyboardModifiers(uint32 native);
void SetXWindowBounds(CefWindowHandle xwindow, int x, int y,
                      int width, int height);
}

#endif // X11UTILS_H

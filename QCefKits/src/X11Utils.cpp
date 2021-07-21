#include "X11Utils.h"
#include <unistd.h>
#include <QX11Info>

namespace QCefKits
{
const char kNetWMPid[] = "_NET_WM_PID";

static Display *getDiaplay()
{
    return QX11Info::display();//cef_get_xdisplay();
}

int XErrorHandlerImpl(Display* display, XErrorEvent* event)
{
    (void)display;

    LOG(WARNING) << "X error received: "
                 << "type " << event->type << ", "
                 << "serial " << event->serial << ", "
                 << "error_code " << static_cast<int>(event->error_code) << ", "
                 << "request_code " << static_cast<int>(event->request_code) << ", "
                 << "minor_code " << static_cast<int>(event->minor_code);
    return 0;
}

int XIOErrorHandlerImpl(Display* display)
{
    (void)display;
    return 0;
}

void SetXErrorHandler()
{
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);
}

void ReparentWindow(CefWindowHandle parent, CefWindowHandle child, CefWindowHandle /*container*/)
{
    ::Display* xdisplay = getDiaplay();
//    DCHECK(xdisplay != nullptr);
    XUnmapWindow(xdisplay, child);
    XReparentWindow(xdisplay, child, parent, 0, 0);
    XMapWindow(xdisplay, child);
    //If call XSetInputFocus the initial focus is abnormal until switch to other window.
//    if (container != 0)
//    {
//        XSetInputFocus(xdisplay, container, RevertToParent, CurrentTime);
//    }
//    else
//    {
//        XSetInputFocus(xdisplay, child, RevertToParent, CurrentTime);
//    }
    XFlush(xdisplay);
}

Qt::KeyboardModifiers NativeToQtKeyboardModifiers(uint32 native)
{
    Qt::KeyboardModifiers qt_mask = Qt::NoModifier;
    if (native & ShiftMask)
    {
        qt_mask |= Qt::ShiftModifier;
    }
    if (native & ControlMask)
    {
        qt_mask |= Qt::ControlModifier;
    }
    if (native & Mod1Mask)
    {
        qt_mask |= Qt::AltModifier;
    }
    if (native & Mod4Mask)
    {
        qt_mask |= Qt::MetaModifier;
    }
    return qt_mask;
}

unsigned long CreateCefBrowserWindow(int width, int height)
{
    ::Display* xdisplay = getDiaplay();
    Window root_window = XRootWindow(xdisplay, 0);
    XSetWindowAttributes swa;
    memset(&swa, 0, sizeof(swa));
    swa.background_pixmap = None;
    swa.override_redirect = false;
    Window window = XCreateWindow(xdisplay, root_window,
                                  0, 0,
                                  static_cast<unsigned int>(width),
                                  static_cast<unsigned int>(height),  // geometry
                                  0,  /* border width*/
                                  CopyFromParent,  /* depth*/
                                  InputOutput,
                                  CopyFromParent,  /* visual */
                                  CWBackPixmap | CWOverrideRedirect,
                                  &swa);
    long event_mask = FocusChangeMask | StructureNotifyMask | PropertyChangeMask;
    XSelectInput(xdisplay, window, event_mask);
    // Add PID flag to window.
    long pid = getpid();
    const Atom pid_atom = XInternAtom(xdisplay, kNetWMPid, false);
    XChangeProperty(xdisplay,
                    window,
                    pid_atom,
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&pid), 1);
    XFlush(xdisplay);
    return window;
}

void DestoryCefBrowserWindow(unsigned long winid)
{
    ::Display* xdisplay = getDiaplay();
    XDestroyWindow(xdisplay, winid);
}

void SetXWindowBounds(CefWindowHandle xwindow, int x, int y,
                      int width, int height)
{
    ::Display* xdisplay = getDiaplay();
    XWindowChanges changes;
    changes.x = x;
    changes.y = y;
    changes.width = width;
    changes.height = height;
    XConfigureWindow(xdisplay, xwindow,
                     CWX | CWY | CWHeight | CWWidth,
                     &changes);
}
}

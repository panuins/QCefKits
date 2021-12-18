TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    cef/$$QMAKE_HOST.os$$QT_ARCH/libcef_dll/cef_dll_wrapper.pro \
    cef/cefclient \
    QCefKits \
    CefSubProcess \
    tests/QCefKitsBrowser \
    tests/QCefKitsSimple

cef/cefclient.depends = cef/$$QMAKE_HOST.os$$QT_ARCH/libcef_dll/cef_dll_wrapper.pro
QCefKits.depends = cef/$$QMAKE_HOST.os$$QT_ARCH/libcef_dll/cef_dll_wrapper.pro
CefSubProcess.depends = cef/$$QMAKE_HOST.os$$QT_ARCH/libcef_dll/cef_dll_wrapper.pro
tests/QCefKitsSimple.depends = QCefKits CefSubProcess

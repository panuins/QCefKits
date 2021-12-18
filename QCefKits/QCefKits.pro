QT += widgets
unix: QT += x11extras
TEMPLATE = lib
DEFINES += QCEFKITS_LIBRARY
DESTDIR = $$OUT_PWD/../libs
DLLDESTDIR = $$OUT_PWD/../bin

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32: QMAKE_CXXFLAGS_WARN_ON -= -w34100
else: QMAKE_CXXFLAGS += -Wno-unused-parameter

CEFPATH=$$_PRO_FILE_PWD_/../cef/$$QMAKE_HOST.os$$QT_ARCH
CEFDLLWRAPPERPATH=$$OUT_PWD/../libs
message($$CEFPATH)

INCLUDEPATH += $$CEFPATH

LIBS += -L$$CEFDLLWRAPPERPATH -lcef_dll_wrapper

CONFIG(debug, debug|release) {
win32: LIBS += -L$$CEFPATH/Debug -llibcef
else: LIBS += -L$$CEFPATH/Debug -lcef
DEPENDPATH += $$CEFPATH/Debug $$CEFDLLWRAPPERPATH
}
CONFIG(release, debug|release) {
win32: LIBS += -L$$CEFPATH/Release -llibcef
else: LIBS += -L$$CEFPATH/Release -lcef
DEPENDPATH += $$CEFPATH/Release $$CEFDLLWRAPPERPATH
}
win32: PRE_TARGETDEPS += $$CEFDLLWRAPPERPATH/cef_dll_wrapper.lib
else: PRE_TARGETDEPS += $$CEFDLLWRAPPERPATH/libcef_dll_wrapper.a

win32: LIBS += d3d11.lib glu32.lib imm32.lib opengl32.lib oleacc.lib comctl32.lib rpcrt4.lib shlwapi.lib ws2_32.lib user32.lib Shell32.lib Gdi32.lib Comdlg32.lib
unix: LIBS += -lX11

DEFINES += WRAPPING_CEF_SHARED
win32:{
DEFINES += WIN32 _WIN32 _WINDOWS
DEFINES += UNICODE _UNICODE
DEFINES += WINVER=0x0601 _WIN32_WINNT=0x601
DEFINES += NOMINMAX
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += _HAS_EXCEPTIONS=0

QMAKE_CXXFLAGS_WARN_ON -= -w34100
}
unix:{
DEFINES += _FILE_OFFSET_BITS=64
QMAKE_CFLAGS += -std=c99
QMAKE_CXXFLAGS += -fno-strict-aliasing
QMAKE_CXXFLAGS += -fPIC
QMAKE_CXXFLAGS += -fstack-protector
QMAKE_CXXFLAGS += -funwind-tables
QMAKE_CXXFLAGS += -fvisibility=hidden
QMAKE_CXXFLAGS += --param=ssp-buffer-size=4
QMAKE_CXXFLAGS += -pipe
QMAKE_CXXFLAGS += -pthread
QMAKE_CXXFLAGS += -fno-exceptions
QMAKE_CXXFLAGS += -fno-rtti
QMAKE_CXXFLAGS += -fno-threadsafe-statics
QMAKE_CXXFLAGS += -fvisibility-inlines-hidden
QMAKE_CXXFLAGS += -std=gnu++11
QMAKE_CXXFLAGS += -Wsign-compare
QMAKE_CXXFLAGS += -Wno-missing-field-initializers
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-error=comment
QMAKE_CXXFLAGS += -Wno-comment
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-literal-suffix
QMAKE_CXXFLAGS += -Wno-narrowing
QMAKE_CXXFLAGS += -Wno-attributes
QMAKE_LFLAGS += -fPIC -pthread
QMAKE_LFLAGS += -Wl,--disable-new-dtags
QMAKE_LFLAGS += -Wl,--fatal-warnings
QMAKE_LFLAGS += -Wl,-rpath,.
QMAKE_LFLAGS += -Wl,-z,noexecstack
QMAKE_LFLAGS += -Wl,-z,now
QMAKE_LFLAGS += -Wl,-z,relro
CONFIG(debug, debug|release){

} else {
DEFINES -= _FORTIFY_SOURCE
DEFINES += _FORTIFY_SOURCE=2
DEFINES += NDEBUG
QMAKE_LFLAGS += -fdata-sections
QMAKE_LFLAGS += -ffunction-sections
QMAKE_LFLAGS += -fno-ident
QMAKE_LFLAGS += -Wl,-O1
QMAKE_LFLAGS += -Wl,--as-needed
QMAKE_LFLAGS += -Wl,--gc-sections
}
contains(QT_ARCH, x86_64) {
QMAKE_CXXFLAGS += -m64 -march=x86-64
QMAKE_LFLAGS += -m64
}
}

SOURCES += \
    src/QCefBrowserBar.cpp \
    src/QCefStackedWidget.cpp \
    src/ClientAppRenderer.cpp \
    src/QCefKits.cpp \
    src/QCefKitsSettings.cpp \
    src/QCefWidget.cpp \
    src/CefSwitches.cpp \
    src/CefWebPage.cpp \
    src/ClientApp.cpp \
    src/ClientAppBrowser.cpp \
    src/ClientBrowserDelegate.cpp \
    src/ClientHandler.cpp \
    src/ClientRequestContextHandler.cpp \
    src/ClientUtils.cpp \
    src/FileDialogHandler.cpp \
    src/JSDialogHandler.cpp \
    src/IPendingDialog.cpp

HEADERS += \
    include/QCefBrowserBar.h \
    include/QCefKits.h \
    include/QCefKitsSettings.h \
    include/QCefStackedWidget.h \
    include/QCefWidget.h \
    include/QCefKitsGlobal.h \
    src/CefSwitches.h \
    src/CefWebPage.h \
    src/ClientApp.h \
    src/ClientAppBrowser.h \
    src/ClientAppRenderer.h \
    src/ClientBrowserDelegate.h \
    src/ClientHandler.h \
    src/ClientRequestContextHandler.h \
    src/ClientType.h \
    src/ClientUtils.h \
    src/FileDialogHandler.h \
    src/JSDialogHandler.h \
    src/IPendingDialog.h \
    src/QCefKits_internal.h

FORMS += \
    src/QCefBrowserBar.ui

# Default rules for deployment.
unix {
SOURCES += \
    src/X11Utils.cpp

HEADERS += \
    src/X11Utils.h

    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = $$OUT_PWD/../bin
CEFPATH=$$_PRO_FILE_PWD_/../cef/$$QMAKE_HOST.os$$QT_ARCH
CEFDLLWRAPPERPATH=$$OUT_PWD/../libs
message($$CEFPATH)

win32: QMAKE_CXXFLAGS_WARN_ON -= -w34100
unix: {
QMAKE_CXXFLAGS += -Wno-unused-parameter
}

INCLUDEPATH += $$CEFPATH

CONFIG(debug, debug|release) {
LIBS += -L$$CEFDLLWRAPPERPATH -lcef_dll_wrapper
win32: LIBS += -L$$CEFPATH/Debug -llibcef
else: LIBS += -L$$CEFPATH/Debug -lcef
DEPENDPATH += $$CEFPATH/Debug $$CEFDLLWRAPPERPATH
}
CONFIG(release, debug|release) {
LIBS += -L$$CEFDLLWRAPPERPATH -lcef_dll_wrapper
win32: LIBS += -L$$CEFPATH/Release -llibcef
else: LIBS += -L$$CEFPATH/Release -lcef
DEPENDPATH += $$CEFPATH/Release $$CEFDLLWRAPPERPATH
}

SOURCES += \
        ClientApp.cpp \
        ClientAppOther.cpp \
        ClientAppRenderer.cpp \
        ClientRenderDelegate.cpp \
        main.cpp

HEADERS += \
    ClientApp.h \
    ClientAppOther.h \
    ClientAppRenderer.h \
    ClientRenderDelegate.h

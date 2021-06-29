QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
DESTDIR = $$OUT_PWD/../../bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$_PRO_FILE_PWD_/../../QCefKits/include

win32:{
LIBS += -L$$OUT_PWD/../../libs -lQCefKits
PRE_TARGETDEPS += $$OUT_PWD/../../libs/QCefKits.lib
PRE_TARGETDEPS += $$OUT_PWD/../../bin/QCefKits.dll
PRE_TARGETDEPS += $$OUT_PWD/../../bin/CefSubProcess.exe
}
else:{
LIBS += -L$$OUT_PWD/../../libs -lQCefKits
PRE_TARGETDEPS += $$OUT_PWD/../../libs/libQCefKits.a
PRE_TARGETDEPS += $$OUT_PWD/../../libs/libQCefKits.so
PRE_TARGETDEPS += $$OUT_PWD/../../bin/CefSubProcess
}

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

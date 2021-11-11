TEMPLATE = app
CONFIG += c++14
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

TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = $$OUT_PWD/../../bin
CEFPATH=$$_PRO_FILE_PWD_/../$$QMAKE_HOST.os$$QT_ARCH
CEFDLLWRAPPERPATH=$$OUT_PWD/../../libs
message($$CEFPATH)

INCLUDEPATH += $$CEFPATH

win32: LIBS += d3d11.lib glu32.lib imm32.lib opengl32.lib oleacc.lib comctl32.lib rpcrt4.lib shlwapi.lib ws2_32.lib user32.lib Shell32.lib Gdi32.lib Comdlg32.lib

CONFIG(debug, debug|release)
{
LIBS += -L$$CEFDLLWRAPPERPATH -lcef_dll_wrapper
win32: LIBS += -L$$CEFPATH/Debug -llibcef
else: LIBS += -L$$CEFPATH/Debug -lcef
DEPENDPATH += $$CEFPATH/Debug $$CEFDLLWRAPPERPATH
}
CONFIG(release, debug|release)
{
LIBS += -L$$CEFDLLWRAPPERPATH -lcef_dll_wrapper
win32:LIBS += -L$$CEFPATH/Release -llibcef
else:LIBS += -L$$CEFPATH/Release -lcef
DEPENDPATH += $$CEFPATH/Release $$CEFDLLWRAPPERPATH
}

SOURCES += \
        browser/binding_test.cc \
        browser/browser_window.cc \
        browser/bytes_write_handler.cc \
        browser/client_app_delegates_browser.cc \
        browser/client_browser.cc \
        browser/client_handler.cc \
        browser/client_handler_osr.cc \
        browser/client_handler_std.cc \
        browser/dialog_test.cc \
        browser/drm_test.cc \
        browser/image_cache.cc \
        browser/main_context.cc \
        browser/main_context_impl.cc \
        browser/media_router_test.cc \
        browser/osr_accessibility_helper.cc \
        browser/osr_accessibility_node.cc \
        browser/osr_renderer.cc \
        browser/preferences_test.cc \
        browser/response_filter_test.cc \
        browser/root_window.cc \
        browser/root_window_create.cc \
        browser/root_window_manager.cc \
        browser/root_window_views.cc \
        browser/scheme_test.cc \
        browser/server_test.cc \
        browser/test_runner.cc \
        browser/urlrequest_test.cc \
        browser/views_menu_bar.cc \
        browser/views_style.cc \
        browser/views_window.cc \
        browser/window_test.cc \
        browser/window_test_runner.cc \
        browser/window_test_runner_views.cc \
        common/client_app_delegates_common.cc \
        common/scheme_test_common.cc \
        renderer/client_app_delegates_renderer.cc \
        renderer/client_renderer.cc \
        renderer/performance_test.cc \
        renderer/performance_test_tests.cc \
        shared/browser/client_app_browser.cc \
        shared/browser/extension_util.cc \
        shared/browser/file_util.cc \
        shared/browser/geometry_util.cc \
        shared/browser/main_message_loop.cc \
        shared/browser/main_message_loop_external_pump.cc \
        shared/browser/main_message_loop_std.cc \
        shared/common/client_app.cc \
        shared/common/client_app_other.cc \
        shared/common/client_switches.cc \
        shared/renderer/client_app_renderer.cc

DISTFILES += \
    resources/binding.html \
    resources/dialogs.html \
    resources/draggable.html \
    resources/drm.html \
    resources/extensions/set_page_color/README.md \
    resources/extensions/set_page_color/icon.png \
    resources/extensions/set_page_color/manifest.json \
    resources/extensions/set_page_color/popup.html \
    resources/extensions/set_page_color/popup.js \
    resources/localstorage.html \
    resources/logo.png \
    resources/media_router.html \
    resources/menu_icon.1x.png \
    resources/menu_icon.2x.png \
    resources/other_tests.html \
    resources/performance.html \
    resources/performance2.html \
    resources/preferences.html \
    resources/response_filter.html \
    resources/server.html \
    resources/transparency.html \
    resources/urlrequest.html \
    resources/websocket.html \
    resources/window.html \
    resources/xmlhttprequest.html \
    shared/resources/osr_test.html \
    shared/resources/pdf.html \
    shared/resources/pdf.pdf \
    shared/resources/window_icon.1x.png \
    shared/resources/window_icon.2x.png

HEADERS += \
    browser/binding_test.h \
    browser/browser_window.h \
    browser/bytes_write_handler.h \
    browser/client_browser.h \
    browser/client_handler.h \
    browser/client_handler_osr.h \
    browser/client_handler_std.h \
    browser/client_types.h \
    browser/dialog_test.h \
    browser/drm_test.h \
    browser/image_cache.h \
    browser/main_context.h \
    browser/main_context_impl.h \
    browser/media_router_test.h \
    browser/osr_accessibility_helper.h \
    browser/osr_accessibility_node.h \
    browser/osr_dragdrop_events.h \
    browser/osr_renderer.h \
    browser/osr_renderer_settings.h \
    browser/preferences_test.h \
    browser/resource.h \
    browser/response_filter_test.h \
    browser/root_window.h \
    browser/root_window_manager.h \
    browser/root_window_views.h \
    browser/scheme_test.h \
    browser/server_test.h \
    browser/temp_window.h \
    browser/test_runner.h \
    browser/urlrequest_test.h \
    browser/views_menu_bar.h \
    browser/views_style.h \
    browser/views_window.h \
    browser/window_test.h \
    browser/window_test_runner.h \
    browser/window_test_runner_views.h \
    common/scheme_test_common.h \
    renderer/client_renderer.h \
    renderer/performance_test.h \
    renderer/performance_test_setup.h \
    shared/browser/client_app_browser.h \
    shared/browser/extension_util.h \
    shared/browser/file_util.h \
    shared/browser/geometry_util.h \
    shared/browser/main_message_loop.h \
    shared/browser/main_message_loop_external_pump.h \
    shared/browser/main_message_loop_std.h \
    shared/browser/resource_util.h \
    shared/common/client_app.h \
    shared/common/client_app_other.h \
    shared/common/client_switches.h \
    shared/renderer/client_app_renderer.h

win32: {
DEFINES += WIN32 _WIN32 _WINDOWS
DEFINES += UNICODE _UNICODE
DEFINES += WINVER=0x0601 _WIN32_WINNT=0x601
DEFINES += NOMINMAX
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += _HAS_EXCEPTIONS=0

RC_INCLUDEPATH += $$_PRO_FILE_PWD_
RC_INCLUDEPATH += $$CEFPATH
QMAKE_CXXFLAGS_WARN_ON -= -w34100

SOURCES += \
browser/browser_window_osr_win.cc \
browser/browser_window_std_win.cc \
browser/main_context_impl_win.cc \
browser/main_message_loop_multithreaded_win.cc \
browser/osr_accessibility_node_win.cc \
browser/osr_d3d11_win.cc \
browser/osr_dragdrop_win.cc \
browser/osr_ime_handler_win.cc \
browser/osr_render_handler_win.cc \
browser/osr_render_handler_win_d3d11.cc \
browser/osr_render_handler_win_gl.cc \
browser/osr_window_win.cc \
browser/resource_util_win_idmap.cc \
browser/root_window_win.cc \
browser/temp_window_win.cc \
browser/window_test_runner_win.cc \
cefclient_win.cc \
shared/browser/main_message_loop_external_pump_win.cc \
shared/browser/resource_util_win.cc \
shared/browser/util_win.cc

HEADERS += \
resources/win/cefclient.rc \
browser/browser_window_osr_win.h \
browser/browser_window_std_win.h \
browser/main_message_loop_multithreaded_win.h \
browser/osr_d3d11_win.h \
browser/osr_dragdrop_win.h \
browser/osr_ime_handler_win.h \
browser/osr_render_handler_win.h \
browser/osr_render_handler_win_d3d11.h \
browser/osr_render_handler_win_gl.h \
browser/osr_window_win.h \
browser/root_window_win.h \
browser/temp_window_win.h \
browser/window_test_runner_win.h \
shared/browser/util_win.h

DISTFILES += \
    resources/win/README-TRANSFER.txt \
    resources/win/cefclient.exe.manifest \
    resources/win/cefclient.ico \
    resources/win/compatibility.manifest \
    resources/win/small.ico

RC_FILE = resources/win/cefclient.rc
}
unix: {
}

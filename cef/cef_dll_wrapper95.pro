CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

DESTDIR = $$OUT_PWD/../../../libs
INCLUDEPATH += $$_PRO_FILE_PWD_/../

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
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

HEADERS += \
    cpptoc/accessibility_handler_cpptoc.h \
    cpptoc/app_cpptoc.h \
    cpptoc/audio_handler_cpptoc.h \
    cpptoc/base_ref_counted_cpptoc.h \
    cpptoc/base_scoped_cpptoc.h \
    cpptoc/browser_process_handler_cpptoc.h \
    cpptoc/client_cpptoc.h \
    cpptoc/completion_callback_cpptoc.h \
    cpptoc/context_menu_handler_cpptoc.h \
    cpptoc/cookie_access_filter_cpptoc.h \
    cpptoc/cookie_visitor_cpptoc.h \
    cpptoc/cpptoc_ref_counted.h \
    cpptoc/cpptoc_scoped.h \
    cpptoc/delete_cookies_callback_cpptoc.h \
    cpptoc/dev_tools_message_observer_cpptoc.h \
    cpptoc/dialog_handler_cpptoc.h \
    cpptoc/display_handler_cpptoc.h \
    cpptoc/domvisitor_cpptoc.h \
    cpptoc/download_handler_cpptoc.h \
    cpptoc/download_image_callback_cpptoc.h \
    cpptoc/drag_handler_cpptoc.h \
    cpptoc/end_tracing_callback_cpptoc.h \
    cpptoc/extension_handler_cpptoc.h \
    cpptoc/find_handler_cpptoc.h \
    cpptoc/focus_handler_cpptoc.h \
    cpptoc/frame_handler_cpptoc.h \
    cpptoc/jsdialog_handler_cpptoc.h \
    cpptoc/keyboard_handler_cpptoc.h \
    cpptoc/life_span_handler_cpptoc.h \
    cpptoc/load_handler_cpptoc.h \
    cpptoc/media_observer_cpptoc.h \
    cpptoc/media_route_create_callback_cpptoc.h \
    cpptoc/media_sink_device_info_callback_cpptoc.h \
    cpptoc/menu_model_delegate_cpptoc.h \
    cpptoc/navigation_entry_visitor_cpptoc.h \
    cpptoc/pdf_print_callback_cpptoc.h \
    cpptoc/print_handler_cpptoc.h \
    cpptoc/read_handler_cpptoc.h \
    cpptoc/render_handler_cpptoc.h \
    cpptoc/render_process_handler_cpptoc.h \
    cpptoc/request_context_handler_cpptoc.h \
    cpptoc/request_handler_cpptoc.h \
    cpptoc/resolve_callback_cpptoc.h \
    cpptoc/resource_bundle_handler_cpptoc.h \
    cpptoc/resource_handler_cpptoc.h \
    cpptoc/resource_request_handler_cpptoc.h \
    cpptoc/response_filter_cpptoc.h \
    cpptoc/run_file_dialog_callback_cpptoc.h \
    cpptoc/scheme_handler_factory_cpptoc.h \
    cpptoc/server_handler_cpptoc.h \
    cpptoc/set_cookie_callback_cpptoc.h \
    cpptoc/string_visitor_cpptoc.h \
    cpptoc/task_cpptoc.h \
    cpptoc/test/translator_test_ref_ptr_client_child_cpptoc.h \
    cpptoc/test/translator_test_ref_ptr_client_cpptoc.h \
    cpptoc/test/translator_test_scoped_client_child_cpptoc.h \
    cpptoc/test/translator_test_scoped_client_cpptoc.h \
    cpptoc/urlrequest_client_cpptoc.h \
    cpptoc/v8accessor_cpptoc.h \
    cpptoc/v8array_buffer_release_callback_cpptoc.h \
    cpptoc/v8handler_cpptoc.h \
    cpptoc/v8interceptor_cpptoc.h \
    cpptoc/views/browser_view_delegate_cpptoc.h \
    cpptoc/views/button_delegate_cpptoc.h \
    cpptoc/views/menu_button_delegate_cpptoc.h \
    cpptoc/views/panel_delegate_cpptoc.h \
    cpptoc/views/textfield_delegate_cpptoc.h \
    cpptoc/views/view_delegate_cpptoc.h \
    cpptoc/views/window_delegate_cpptoc.h \
    cpptoc/web_plugin_info_visitor_cpptoc.h \
    cpptoc/web_plugin_unstable_callback_cpptoc.h \
    cpptoc/write_handler_cpptoc.h \
    ctocpp/auth_callback_ctocpp.h \
    ctocpp/before_download_callback_ctocpp.h \
    ctocpp/binary_value_ctocpp.h \
    ctocpp/browser_ctocpp.h \
    ctocpp/browser_host_ctocpp.h \
    ctocpp/callback_ctocpp.h \
    ctocpp/command_line_ctocpp.h \
    ctocpp/context_menu_params_ctocpp.h \
    ctocpp/cookie_manager_ctocpp.h \
    ctocpp/ctocpp_ref_counted.h \
    ctocpp/ctocpp_scoped.h \
    ctocpp/dictionary_value_ctocpp.h \
    ctocpp/domdocument_ctocpp.h \
    ctocpp/domnode_ctocpp.h \
    ctocpp/download_item_callback_ctocpp.h \
    ctocpp/download_item_ctocpp.h \
    ctocpp/drag_data_ctocpp.h \
    ctocpp/extension_ctocpp.h \
    ctocpp/file_dialog_callback_ctocpp.h \
    ctocpp/frame_ctocpp.h \
    ctocpp/get_extension_resource_callback_ctocpp.h \
    ctocpp/image_ctocpp.h \
    ctocpp/jsdialog_callback_ctocpp.h \
    ctocpp/list_value_ctocpp.h \
    ctocpp/media_route_ctocpp.h \
    ctocpp/media_router_ctocpp.h \
    ctocpp/media_sink_ctocpp.h \
    ctocpp/media_source_ctocpp.h \
    ctocpp/menu_model_ctocpp.h \
    ctocpp/navigation_entry_ctocpp.h \
    ctocpp/post_data_ctocpp.h \
    ctocpp/post_data_element_ctocpp.h \
    ctocpp/print_dialog_callback_ctocpp.h \
    ctocpp/print_job_callback_ctocpp.h \
    ctocpp/print_settings_ctocpp.h \
    ctocpp/process_message_ctocpp.h \
    ctocpp/registration_ctocpp.h \
    ctocpp/request_context_ctocpp.h \
    ctocpp/request_ctocpp.h \
    ctocpp/resource_bundle_ctocpp.h \
    ctocpp/resource_read_callback_ctocpp.h \
    ctocpp/resource_skip_callback_ctocpp.h \
    ctocpp/response_ctocpp.h \
    ctocpp/run_context_menu_callback_ctocpp.h \
    ctocpp/scheme_registrar_ctocpp.h \
    ctocpp/select_client_certificate_callback_ctocpp.h \
    ctocpp/server_ctocpp.h \
    ctocpp/sslinfo_ctocpp.h \
    ctocpp/sslstatus_ctocpp.h \
    ctocpp/stream_reader_ctocpp.h \
    ctocpp/stream_writer_ctocpp.h \
    ctocpp/task_runner_ctocpp.h \
    ctocpp/test/translator_test_ctocpp.h \
    ctocpp/test/translator_test_ref_ptr_library_child_child_ctocpp.h \
    ctocpp/test/translator_test_ref_ptr_library_child_ctocpp.h \
    ctocpp/test/translator_test_ref_ptr_library_ctocpp.h \
    ctocpp/test/translator_test_scoped_library_child_child_ctocpp.h \
    ctocpp/test/translator_test_scoped_library_child_ctocpp.h \
    ctocpp/test/translator_test_scoped_library_ctocpp.h \
    ctocpp/thread_ctocpp.h \
    ctocpp/urlrequest_ctocpp.h \
    ctocpp/v8context_ctocpp.h \
    ctocpp/v8exception_ctocpp.h \
    ctocpp/v8stack_frame_ctocpp.h \
    ctocpp/v8stack_trace_ctocpp.h \
    ctocpp/v8value_ctocpp.h \
    ctocpp/value_ctocpp.h \
    ctocpp/views/box_layout_ctocpp.h \
    ctocpp/views/browser_view_ctocpp.h \
    ctocpp/views/button_ctocpp.h \
    ctocpp/views/display_ctocpp.h \
    ctocpp/views/fill_layout_ctocpp.h \
    ctocpp/views/label_button_ctocpp.h \
    ctocpp/views/layout_ctocpp.h \
    ctocpp/views/menu_button_ctocpp.h \
    ctocpp/views/menu_button_pressed_lock_ctocpp.h \
    ctocpp/views/overlay_controller_ctocpp.h \
    ctocpp/views/panel_ctocpp.h \
    ctocpp/views/scroll_view_ctocpp.h \
    ctocpp/views/textfield_ctocpp.h \
    ctocpp/views/view_ctocpp.h \
    ctocpp/views/window_ctocpp.h \
    ctocpp/waitable_event_ctocpp.h \
    ctocpp/web_plugin_info_ctocpp.h \
    ctocpp/x509cert_principal_ctocpp.h \
    ctocpp/x509certificate_ctocpp.h \
    ctocpp/xml_reader_ctocpp.h \
    ctocpp/zip_reader_ctocpp.h \
    shutdown_checker.h \
    transfer_util.h \
    wrapper/cef_browser_info_map.h \
    wrapper_types.h

SOURCES += \
    base/cef_atomic_flag.cc \
    base/cef_callback_helpers.cc \
    base/cef_callback_internal.cc \
    base/cef_lock.cc \
    base/cef_lock_impl.cc \
    base/cef_logging.cc \
    base/cef_ref_counted.cc \
    base/cef_thread_checker_impl.cc \
    base/cef_weak_ptr.cc \
    cpptoc/accessibility_handler_cpptoc.cc \
    cpptoc/app_cpptoc.cc \
    cpptoc/audio_handler_cpptoc.cc \
    cpptoc/base_ref_counted_cpptoc.cc \
    cpptoc/base_scoped_cpptoc.cc \
    cpptoc/browser_process_handler_cpptoc.cc \
    cpptoc/client_cpptoc.cc \
    cpptoc/completion_callback_cpptoc.cc \
    cpptoc/context_menu_handler_cpptoc.cc \
    cpptoc/cookie_access_filter_cpptoc.cc \
    cpptoc/cookie_visitor_cpptoc.cc \
    cpptoc/delete_cookies_callback_cpptoc.cc \
    cpptoc/dev_tools_message_observer_cpptoc.cc \
    cpptoc/dialog_handler_cpptoc.cc \
    cpptoc/display_handler_cpptoc.cc \
    cpptoc/domvisitor_cpptoc.cc \
    cpptoc/download_handler_cpptoc.cc \
    cpptoc/download_image_callback_cpptoc.cc \
    cpptoc/drag_handler_cpptoc.cc \
    cpptoc/end_tracing_callback_cpptoc.cc \
    cpptoc/extension_handler_cpptoc.cc \
    cpptoc/find_handler_cpptoc.cc \
    cpptoc/focus_handler_cpptoc.cc \
    cpptoc/frame_handler_cpptoc.cc \
    cpptoc/jsdialog_handler_cpptoc.cc \
    cpptoc/keyboard_handler_cpptoc.cc \
    cpptoc/life_span_handler_cpptoc.cc \
    cpptoc/load_handler_cpptoc.cc \
    cpptoc/media_observer_cpptoc.cc \
    cpptoc/media_route_create_callback_cpptoc.cc \
    cpptoc/media_sink_device_info_callback_cpptoc.cc \
    cpptoc/menu_model_delegate_cpptoc.cc \
    cpptoc/navigation_entry_visitor_cpptoc.cc \
    cpptoc/pdf_print_callback_cpptoc.cc \
    cpptoc/print_handler_cpptoc.cc \
    cpptoc/read_handler_cpptoc.cc \
    cpptoc/render_handler_cpptoc.cc \
    cpptoc/render_process_handler_cpptoc.cc \
    cpptoc/request_context_handler_cpptoc.cc \
    cpptoc/request_handler_cpptoc.cc \
    cpptoc/resolve_callback_cpptoc.cc \
    cpptoc/resource_bundle_handler_cpptoc.cc \
    cpptoc/resource_handler_cpptoc.cc \
    cpptoc/resource_request_handler_cpptoc.cc \
    cpptoc/response_filter_cpptoc.cc \
    cpptoc/run_file_dialog_callback_cpptoc.cc \
    cpptoc/scheme_handler_factory_cpptoc.cc \
    cpptoc/server_handler_cpptoc.cc \
    cpptoc/set_cookie_callback_cpptoc.cc \
    cpptoc/string_visitor_cpptoc.cc \
    cpptoc/task_cpptoc.cc \
    cpptoc/test/translator_test_ref_ptr_client_child_cpptoc.cc \
    cpptoc/test/translator_test_ref_ptr_client_cpptoc.cc \
    cpptoc/test/translator_test_scoped_client_child_cpptoc.cc \
    cpptoc/test/translator_test_scoped_client_cpptoc.cc \
    cpptoc/urlrequest_client_cpptoc.cc \
    cpptoc/v8accessor_cpptoc.cc \
    cpptoc/v8array_buffer_release_callback_cpptoc.cc \
    cpptoc/v8handler_cpptoc.cc \
    cpptoc/v8interceptor_cpptoc.cc \
    cpptoc/views/browser_view_delegate_cpptoc.cc \
    cpptoc/views/button_delegate_cpptoc.cc \
    cpptoc/views/menu_button_delegate_cpptoc.cc \
    cpptoc/views/panel_delegate_cpptoc.cc \
    cpptoc/views/textfield_delegate_cpptoc.cc \
    cpptoc/views/view_delegate_cpptoc.cc \
    cpptoc/views/window_delegate_cpptoc.cc \
    cpptoc/web_plugin_info_visitor_cpptoc.cc \
    cpptoc/web_plugin_unstable_callback_cpptoc.cc \
    cpptoc/write_handler_cpptoc.cc \
    ctocpp/auth_callback_ctocpp.cc \
    ctocpp/before_download_callback_ctocpp.cc \
    ctocpp/binary_value_ctocpp.cc \
    ctocpp/browser_ctocpp.cc \
    ctocpp/browser_host_ctocpp.cc \
    ctocpp/callback_ctocpp.cc \
    ctocpp/command_line_ctocpp.cc \
    ctocpp/context_menu_params_ctocpp.cc \
    ctocpp/cookie_manager_ctocpp.cc \
    ctocpp/dictionary_value_ctocpp.cc \
    ctocpp/domdocument_ctocpp.cc \
    ctocpp/domnode_ctocpp.cc \
    ctocpp/download_item_callback_ctocpp.cc \
    ctocpp/download_item_ctocpp.cc \
    ctocpp/drag_data_ctocpp.cc \
    ctocpp/extension_ctocpp.cc \
    ctocpp/file_dialog_callback_ctocpp.cc \
    ctocpp/frame_ctocpp.cc \
    ctocpp/get_extension_resource_callback_ctocpp.cc \
    ctocpp/image_ctocpp.cc \
    ctocpp/jsdialog_callback_ctocpp.cc \
    ctocpp/list_value_ctocpp.cc \
    ctocpp/media_route_ctocpp.cc \
    ctocpp/media_router_ctocpp.cc \
    ctocpp/media_sink_ctocpp.cc \
    ctocpp/media_source_ctocpp.cc \
    ctocpp/menu_model_ctocpp.cc \
    ctocpp/navigation_entry_ctocpp.cc \
    ctocpp/post_data_ctocpp.cc \
    ctocpp/post_data_element_ctocpp.cc \
    ctocpp/print_dialog_callback_ctocpp.cc \
    ctocpp/print_job_callback_ctocpp.cc \
    ctocpp/print_settings_ctocpp.cc \
    ctocpp/process_message_ctocpp.cc \
    ctocpp/registration_ctocpp.cc \
    ctocpp/request_context_ctocpp.cc \
    ctocpp/request_ctocpp.cc \
    ctocpp/resource_bundle_ctocpp.cc \
    ctocpp/resource_read_callback_ctocpp.cc \
    ctocpp/resource_skip_callback_ctocpp.cc \
    ctocpp/response_ctocpp.cc \
    ctocpp/run_context_menu_callback_ctocpp.cc \
    ctocpp/scheme_registrar_ctocpp.cc \
    ctocpp/select_client_certificate_callback_ctocpp.cc \
    ctocpp/server_ctocpp.cc \
    ctocpp/sslinfo_ctocpp.cc \
    ctocpp/sslstatus_ctocpp.cc \
    ctocpp/stream_reader_ctocpp.cc \
    ctocpp/stream_writer_ctocpp.cc \
    ctocpp/task_runner_ctocpp.cc \
    ctocpp/test/translator_test_ctocpp.cc \
    ctocpp/test/translator_test_ref_ptr_library_child_child_ctocpp.cc \
    ctocpp/test/translator_test_ref_ptr_library_child_ctocpp.cc \
    ctocpp/test/translator_test_ref_ptr_library_ctocpp.cc \
    ctocpp/test/translator_test_scoped_library_child_child_ctocpp.cc \
    ctocpp/test/translator_test_scoped_library_child_ctocpp.cc \
    ctocpp/test/translator_test_scoped_library_ctocpp.cc \
    ctocpp/thread_ctocpp.cc \
    ctocpp/urlrequest_ctocpp.cc \
    ctocpp/v8context_ctocpp.cc \
    ctocpp/v8exception_ctocpp.cc \
    ctocpp/v8stack_frame_ctocpp.cc \
    ctocpp/v8stack_trace_ctocpp.cc \
    ctocpp/v8value_ctocpp.cc \
    ctocpp/value_ctocpp.cc \
    ctocpp/views/box_layout_ctocpp.cc \
    ctocpp/views/browser_view_ctocpp.cc \
    ctocpp/views/button_ctocpp.cc \
    ctocpp/views/display_ctocpp.cc \
    ctocpp/views/fill_layout_ctocpp.cc \
    ctocpp/views/label_button_ctocpp.cc \
    ctocpp/views/layout_ctocpp.cc \
    ctocpp/views/menu_button_ctocpp.cc \
    ctocpp/views/menu_button_pressed_lock_ctocpp.cc \
    ctocpp/views/overlay_controller_ctocpp.cc \
    ctocpp/views/panel_ctocpp.cc \
    ctocpp/views/scroll_view_ctocpp.cc \
    ctocpp/views/textfield_ctocpp.cc \
    ctocpp/views/view_ctocpp.cc \
    ctocpp/views/window_ctocpp.cc \
    ctocpp/waitable_event_ctocpp.cc \
    ctocpp/web_plugin_info_ctocpp.cc \
    ctocpp/x509cert_principal_ctocpp.cc \
    ctocpp/x509certificate_ctocpp.cc \
    ctocpp/xml_reader_ctocpp.cc \
    ctocpp/zip_reader_ctocpp.cc \
    shutdown_checker.cc \
    transfer_util.cc \
    wrapper/cef_byte_read_handler.cc \
    wrapper/cef_closure_task.cc \
    wrapper/cef_message_router.cc \
    wrapper/cef_resource_manager.cc \
    wrapper/cef_scoped_temp_dir.cc \
    wrapper/cef_stream_resource_handler.cc \
    wrapper/cef_xml_object.cc \
    wrapper/cef_zip_archive.cc \
    wrapper/libcef_dll_wrapper.cc \
    wrapper/libcef_dll_wrapper2.cc

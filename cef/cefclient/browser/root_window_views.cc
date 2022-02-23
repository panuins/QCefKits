// Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "browser/root_window_views.h"

#include <memory>

#include "include/cef_version.h"
#if CHROME_VERSION_MAJOR > 94
#include "include/base/cef_callback.h"
#else
#include "include/base/cef_bind.h"
#endif
#include "include/base/cef_build.h"
#if CHROME_VERSION_MAJOR > 94
#include "include/base/cef_callback.h"
#include "include/base/cef_cxx17_backports.h"
#endif
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "browser/client_handler_std.h"

namespace client {

namespace {

// Images that are loaded early and cached.
static const char* kDefaultImageCache[] = {"menu_icon", "window_icon"};

}  // namespace

RootWindowViews::RootWindowViews()
    : is_popup_(false),
      position_on_resize_(false),
      initialized_(false),
      window_destroyed_(false),
      browser_destroyed_(false) {}

RootWindowViews::~RootWindowViews() {
  REQUIRE_MAIN_THREAD();
}

void RootWindowViews::Init(RootWindow::Delegate* delegate,
#if CHROME_VERSION_MAJOR > 94
                           std::unique_ptr<RootWindowConfig> config,
#else
                           const RootWindowConfig& config,
#endif
                           const CefBrowserSettings& settings) {
  DCHECK(delegate);
#if CHROME_VERSION_MAJOR > 94
  DCHECK(!config->with_osr);  // Windowless rendering is not supported.
#else
  DCHECK(!config.with_osr);  // Windowless rendering is not supported.
#endif
  DCHECK(!initialized_);

  delegate_ = delegate;
#if CHROME_VERSION_MAJOR > 94
  config_ = std::move(config);
  if (config_->initially_hidden && !config_->source_bounds.IsEmpty()) {
    // The window will be sized and positioned in OnAutoResize().
    initial_bounds_ = config_->source_bounds;
    position_on_resize_ = true;
  } else {
    initial_bounds_ = config_->bounds;
  }

  CreateClientHandler(config_->url);
  initialized_ = true;

  // Continue initialization on the UI thread.
  InitOnUIThread(settings, config_->url, delegate_->GetRequestContext(this));
#else
  with_controls_ = config.with_controls;
  always_on_top_ = config.always_on_top;
  with_extension_ = config.with_extension;
  initially_hidden_ = config.initially_hidden;
  if (initially_hidden_ && !config.source_bounds.IsEmpty()) {
    // The window will be sized and positioned in OnAutoResize().
    initial_bounds_ = config.source_bounds;
    position_on_resize_ = true;
  } else {
    initial_bounds_ = config.bounds;
  }
  parent_window_ = config.parent_window;
  close_callback_ = config.close_callback;

  CreateClientHandler(config.url);
  initialized_ = true;

  // Continue initialization on the UI thread.
  InitOnUIThread(settings, config.url, delegate_->GetRequestContext(this));
#endif
}

void RootWindowViews::InitAsPopup(RootWindow::Delegate* delegate,
                                  bool with_controls,
                                  bool with_osr,
                                  const CefPopupFeatures& popupFeatures,
                                  CefWindowInfo& windowInfo,
                                  CefRefPtr<CefClient>& client,
                                  CefBrowserSettings& settings) {
  CEF_REQUIRE_UI_THREAD();

  DCHECK(delegate);
  DCHECK(!with_osr);  // Windowless rendering is not supported.
  DCHECK(!initialized_);

  delegate_ = delegate;
#if CHROME_VERSION_MAJOR > 94
  config_ = std::make_unique<RootWindowConfig>();
  config_->with_controls = with_controls;
#else
  with_controls_ = with_controls;
#endif
  is_popup_ = true;

  if (popupFeatures.xSet)
    initial_bounds_.x = popupFeatures.x;
  if (popupFeatures.ySet)
    initial_bounds_.y = popupFeatures.y;
  if (popupFeatures.widthSet)
    initial_bounds_.width = popupFeatures.width;
  if (popupFeatures.heightSet)
    initial_bounds_.height = popupFeatures.height;

  CreateClientHandler(std::string());
  initialized_ = true;

  // The Window will be created in ViewsWindow::OnPopupBrowserViewCreated().
  client = client_handler_;
}

void RootWindowViews::Show(ShowMode mode) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::Show, this, mode));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::Show, this, mode));
#endif
    return;
  }

  if (!window_)
    return;

  window_->Show();

  switch (mode) {
    case ShowMinimized:
      window_->Minimize();
      break;
    case ShowMaximized:
      window_->Maximize();
      break;
    default:
      break;
  }
}

void RootWindowViews::Hide() {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::Hide, this));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::Hide, this));
#endif
    return;
  }

  if (window_)
    window_->Hide();
}

void RootWindowViews::SetBounds(int x, int y, size_t width, size_t height) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::SetBounds, this, x, y,
                                       width, height));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::SetBounds, this, x, y,
                                   width, height));
#endif
    return;
  }

  if (window_) {
    window_->SetBounds(
        CefRect(x, y, static_cast<int>(width), static_cast<int>(height)));
  }
}

void RootWindowViews::Close(bool force) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::Close, this, force));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::Close, this, force));
#endif
    return;
  }

  if (window_)
    window_->Close(force);
}

void RootWindowViews::SetDeviceScaleFactor(float device_scale_factor) {
  REQUIRE_MAIN_THREAD();
  // Windowless rendering is not supported.
  NOTREACHED();
}

float RootWindowViews::GetDeviceScaleFactor() const {
  REQUIRE_MAIN_THREAD();
  // Windowless rendering is not supported.
  NOTREACHED();
  return 0.0;
}

CefRefPtr<CefBrowser> RootWindowViews::GetBrowser() const {
  REQUIRE_MAIN_THREAD();
  return browser_;
}

ClientWindowHandle RootWindowViews::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();
#if defined(OS_LINUX)
  // ClientWindowHandle is a GtkWidget* on Linux and we don't have one of those.
  return nullptr;
#else
  if (browser_)
    return browser_->GetHost()->GetWindowHandle();
  return kNullWindowHandle;
#endif
}

bool RootWindowViews::WithExtension() const {
  DCHECK(initialized_);
#if CHROME_VERSION_MAJOR > 94
  return config_->with_extension;
#else
  return with_extension_;
#endif
}

bool RootWindowViews::WithControls() {
  DCHECK(initialized_);
#if CHROME_VERSION_MAJOR > 94
  return config_->with_controls;
#else
  return with_controls_;
#endif
}

bool RootWindowViews::WithExtension() {
  DCHECK(initialized_);
#if CHROME_VERSION_MAJOR > 94
  return config_->with_extension;
#else
  return with_extension_;
#endif
}

void RootWindowViews::OnExtensionsChanged(const ExtensionSet& extensions) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnExtensionsChanged, this,
                                       extensions));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::OnExtensionsChanged, this,
                                   extensions));
#endif
    return;
  }

  if (window_) {
    window_->OnExtensionsChanged(extensions);
  } else {
    // Window may not exist yet for popups.
    pending_extensions_ = extensions;
  }
}

bool RootWindowViews::InitiallyHidden() {
  CEF_REQUIRE_UI_THREAD();
#if CHROME_VERSION_MAJOR > 94
  return config_->initially_hidden;
#else
  return initially_hidden_;
#endif
}

CefRefPtr<CefWindow> RootWindowViews::GetParentWindow() {
  CEF_REQUIRE_UI_THREAD();
#if CHROME_VERSION_MAJOR > 94
  return config_->parent_window;
#else
  return parent_window_;
#endif
}

CefRect RootWindowViews::GetWindowBounds() {
  CEF_REQUIRE_UI_THREAD();
  return initial_bounds_;
}

scoped_refptr<ImageCache> RootWindowViews::GetImageCache() {
  CEF_REQUIRE_UI_THREAD();
  return image_cache_;
}

void RootWindowViews::OnViewsWindowCreated(CefRefPtr<ViewsWindow> window) {
  CEF_REQUIRE_UI_THREAD();
  DCHECK(!window_);
  window_ = window;
#if CHROME_VERSION_MAJOR > 94
  window_->SetAlwaysOnTop(config_->always_on_top);
#else
  window_->SetAlwaysOnTop(always_on_top_);
#endif

  if (!pending_extensions_.empty()) {
    window_->OnExtensionsChanged(pending_extensions_);
    pending_extensions_.clear();
  }
}

void RootWindowViews::OnViewsWindowDestroyed(CefRefPtr<ViewsWindow> window) {
  CEF_REQUIRE_UI_THREAD();
  window_ = nullptr;

  // Continue on the main thread.
#if CHROME_VERSION_MAJOR > 94
  MAIN_POST_CLOSURE(
      base::BindOnce(&RootWindowViews::NotifyViewsWindowDestroyed, this));
#else
  MAIN_POST_CLOSURE(
      base::Bind(&RootWindowViews::NotifyViewsWindowDestroyed, this));
#endif
}

void RootWindowViews::OnViewsWindowActivated(CefRefPtr<ViewsWindow> window) {
  CEF_REQUIRE_UI_THREAD();

  // Continue on the main thread.
#if CHROME_VERSION_MAJOR > 94
  MAIN_POST_CLOSURE(
      base::BindOnce(&RootWindowViews::NotifyViewsWindowActivated, this));
#else
  MAIN_POST_CLOSURE(
      base::Bind(&RootWindowViews::NotifyViewsWindowActivated, this));
#endif
}

ViewsWindow::Delegate* RootWindowViews::GetDelegateForPopup(
    CefRefPtr<CefClient> client) {
  CEF_REQUIRE_UI_THREAD();
  // |handler| was created in RootWindowViews::InitAsPopup().
  ClientHandlerStd* handler = static_cast<ClientHandlerStd*>(client.get());
  RootWindowViews* root_window =
      static_cast<RootWindowViews*>(handler->delegate());

  // Transfer some state to the child RootWindowViews.
  root_window->image_cache_ = image_cache_;

  return root_window;
}

void RootWindowViews::CreateExtensionWindow(
    CefRefPtr<CefExtension> extension,
    const CefRect& source_bounds,
    CefRefPtr<CefWindow> parent_window,
#if CHROME_VERSION_MAJOR > 94
    base::OnceClosure close_callback) {
#else
    const base::Closure& close_callback) {
#endif
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
#if CHROME_VERSION_MAJOR > 94
    MAIN_POST_CLOSURE(base::BindOnce(&RootWindowViews::CreateExtensionWindow, this,
                                     extension, source_bounds, parent_window,
                                     std::move(close_callback)));
#else
    MAIN_POST_CLOSURE(base::Bind(&RootWindowViews::CreateExtensionWindow, this,
                                 extension, source_bounds, parent_window,
                                 close_callback));
#endif
    return;
  }

  delegate_->CreateExtensionWindow(extension, source_bounds, parent_window,
                                   std::move(close_callback), false);
}

void RootWindowViews::OnTest(int test_id) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
#if CHROME_VERSION_MAJOR > 94
    MAIN_POST_CLOSURE(base::BindOnce(&RootWindowViews::OnTest, this, test_id));
#else
    MAIN_POST_CLOSURE(base::Bind(&RootWindowViews::OnTest, this, test_id));
#endif
    return;
  }

  delegate_->OnTest(this, test_id);
}

void RootWindowViews::OnExit() {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
#if CHROME_VERSION_MAJOR > 94
    MAIN_POST_CLOSURE(base::BindOnce(&RootWindowViews::OnExit, this));
#else
    MAIN_POST_CLOSURE(base::Bind(&RootWindowViews::OnExit, this));
#endif
    return;
  }

  delegate_->OnExit(this);
}

void RootWindowViews::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_MAIN_THREAD();
  DCHECK(!browser_);
  browser_ = browser;
  delegate_->OnBrowserCreated(this, browser);
}

void RootWindowViews::OnBrowserClosing(CefRefPtr<CefBrowser> browser) {
  REQUIRE_MAIN_THREAD();
  // Nothing to do here.
}

void RootWindowViews::OnBrowserClosed(CefRefPtr<CefBrowser> browser) {
  REQUIRE_MAIN_THREAD();
  if (browser_) {
    DCHECK_EQ(browser->GetIdentifier(), browser_->GetIdentifier());
    browser_ = nullptr;
  }

  client_handler_->DetachDelegate();
  client_handler_ = nullptr;

  browser_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowViews::OnSetAddress(const std::string& url) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnSetAddress, this, url));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::OnSetAddress, this, url));
#endif
    return;
  }

#if CHROME_VERSION_MAJOR > 94
  if (window_)
#else
  if (window_ && with_controls_)
#endif
    window_->SetAddress(url);
}

void RootWindowViews::OnSetTitle(const std::string& title) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnSetTitle, this, title));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::OnSetTitle, this, title));
#endif
    return;
  }

  if (window_)
    window_->SetTitle(title);
}

void RootWindowViews::OnSetFavicon(CefRefPtr<CefImage> image) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnSetFavicon, this, image));
#else
    CefPostTask(TID_UI,
                base::Bind(&RootWindowViews::OnSetFavicon, this, image));
#endif
    return;
  }

  if (window_)
    window_->SetFavicon(image);
}

void RootWindowViews::OnSetFullscreen(bool fullscreen) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnSetFullscreen, this,
                                       fullscreen));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::OnSetFullscreen, this,
                                   fullscreen));
#endif
    return;
  }

  if (window_)
    window_->SetFullscreen(fullscreen);
}

void RootWindowViews::OnAutoResize(const CefSize& new_size) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnAutoResize, this, new_size));
#else
    CefPostTask(TID_UI,
                base::Bind(&RootWindowViews::OnAutoResize, this, new_size));
#endif
    return;
  }

  bool has_position = false;
  CefPoint position;
  if (position_on_resize_) {
    // Position the window centered on and immediately below the source.
    const int x_offset = (initial_bounds_.width - new_size.width) / 2;
    position.Set(initial_bounds_.x + x_offset,
                 initial_bounds_.y + initial_bounds_.height);
    has_position = true;

    // Don't change the window position on future resizes.
    position_on_resize_ = false;
  }

  if (window_) {
    window_->SetBrowserSize(new_size, has_position, position);
    window_->Show();
  }
}

void RootWindowViews::OnSetLoadingState(bool isLoading,
                                        bool canGoBack,
                                        bool canGoForward) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnSetLoadingState, this,
                                       isLoading, canGoBack, canGoForward));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::OnSetLoadingState, this,
                                   isLoading, canGoBack, canGoForward));
#endif
    return;
  }

  if (window_) {
#if CHROME_VERSION_MAJOR < 95
    if (with_controls_)
#endif
      window_->SetLoadingState(isLoading, canGoBack, canGoForward);

    if (isLoading) {
      // Reset to the default window icon when loading begins.
      window_->SetFavicon(
          delegate_->GetImageCache()->GetCachedImage("window_icon"));
    }
  }
}

void RootWindowViews::OnSetDraggableRegions(
    const std::vector<CefDraggableRegion>& regions) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnSetDraggableRegions,
                                       this, regions));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::OnSetDraggableRegions,
                                   this, regions));
#endif
    return;
  }

  if (window_)
    window_->SetDraggableRegions(regions);
}

void RootWindowViews::OnTakeFocus(bool next) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnTakeFocus, this, next));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::OnTakeFocus, this, next));
#endif
    return;
  }

  if (window_)
    window_->TakeFocus(next);
}

void RootWindowViews::OnBeforeContextMenu(CefRefPtr<CefMenuModel> model) {
  CEF_REQUIRE_UI_THREAD();
  if (window_)
    window_->OnBeforeContextMenu(model);
}

void RootWindowViews::CreateClientHandler(const std::string& url) {
  DCHECK(!client_handler_);

  client_handler_ = new ClientHandlerStd(this, url);
  client_handler_->set_download_favicon_images(true);
}

void RootWindowViews::InitOnUIThread(
    const CefBrowserSettings& settings,
    const std::string& startup_url,
    CefRefPtr<CefRequestContext> request_context) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
#if CHROME_VERSION_MAJOR > 94
    CefPostTask(TID_UI, base::BindOnce(&RootWindowViews::InitOnUIThread, this,
                                       settings, startup_url, request_context));
#else
    CefPostTask(TID_UI, base::Bind(&RootWindowViews::InitOnUIThread, this,
                                   settings, startup_url, request_context));
#endif
    return;
  }

  image_cache_ = delegate_->GetImageCache();

  // Populate the default image cache.
  ImageCache::ImageInfoSet image_set;
#if CHROME_VERSION_MAJOR > 94
  for (size_t i = 0U; i < base::size(kDefaultImageCache); ++i)
    image_set.push_back(ImageCache::ImageInfo::Create2x(kDefaultImageCache[i]));

  image_cache_->LoadImages(
      image_set, base::BindOnce(&RootWindowViews::CreateViewsWindow, this, settings,
                                startup_url, request_context));
#else
  for (size_t i = 0U; i < arraysize(kDefaultImageCache); ++i)
    image_set.push_back(ImageCache::ImageInfo::Create2x(kDefaultImageCache[i]));

  image_cache_->LoadImages(
      image_set, base::Bind(&RootWindowViews::CreateViewsWindow, this, settings,
                            startup_url, request_context));
#endif
}

void RootWindowViews::CreateViewsWindow(
    const CefBrowserSettings& settings,
    const std::string& startup_url,
    CefRefPtr<CefRequestContext> request_context,
    const ImageCache::ImageSet& images) {
  CEF_REQUIRE_UI_THREAD();
  DCHECK(!window_);

#ifndef NDEBUG
  // Make sure the default images loaded successfully.
  DCHECK_EQ(images.size(), sizeof(kDefaultImageCache)/sizeof(const char*));
  for (size_t i = 0U; i < sizeof(kDefaultImageCache)/sizeof(const char*); ++i) {
    DCHECK(images[i]) << "Default image " << i << " failed to load";
  }
#endif

  // Create the ViewsWindow. It will show itself after creation.
  ViewsWindow::Create(this, client_handler_, startup_url, settings,
                      request_context);
}

void RootWindowViews::NotifyViewsWindowDestroyed() {
  REQUIRE_MAIN_THREAD();
  window_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowViews::NotifyViewsWindowActivated() {
  REQUIRE_MAIN_THREAD();
  delegate_->OnRootWindowActivated(this);
}

void RootWindowViews::NotifyDestroyedIfDone() {
  // Notify once both the window and the browser have been destroyed.
  if (window_destroyed_ && browser_destroyed_) {
    delegate_->OnRootWindowDestroyed(this);
#if CHROME_VERSION_MAJOR > 94
    if (!config_->close_callback.is_null())
      std::move(config_->close_callback).Run();
#else
    if (!close_callback_.is_null())
      close_callback_.Run();
#endif
  }
}

}  // namespace client

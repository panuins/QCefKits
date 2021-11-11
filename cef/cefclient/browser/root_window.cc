// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "browser/root_window.h"

#if CHROME_VERSION_MAJOR > 94
#include "include/base/cef_callback_helpers.h"
#endif
#include "browser/main_context.h"
#include "browser/root_window_manager.h"

namespace client {

RootWindowConfig::RootWindowConfig()
    : always_on_top(false),
      with_controls(true),
      with_osr(false),
      with_extension(false),
      initially_hidden(false),
      url(MainContext::Get()->GetMainURL()) {}

RootWindow::RootWindow() : delegate_(nullptr) {}

RootWindow::~RootWindow() {}

// static
scoped_refptr<RootWindow> RootWindow::GetForBrowser(int browser_id) {
  return MainContext::Get()->GetRootWindowManager()->GetWindowForBrowser(
      browser_id);
}

void RootWindow::OnExtensionsChanged(const ExtensionSet& extensions) {
  REQUIRE_MAIN_THREAD();
  DCHECK(delegate_);
  DCHECK(!WithExtension());

  if (extensions.empty())
    return;

  ExtensionSet::const_iterator it = extensions.begin();
  for (; it != extensions.end(); ++it) {
#if CHROME_VERSION_MAJOR > 94
      delegate_->CreateExtensionWindow(*it, CefRect(), nullptr, base::DoNothing(),
#else
      delegate_->CreateExtensionWindow(*it, CefRect(), nullptr, base::Closure(),
#endif
                                     WithWindowlessRendering());
  }
}

}  // namespace client

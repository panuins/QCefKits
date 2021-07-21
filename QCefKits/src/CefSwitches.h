/****************************************************************************
** QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the LGPLV2 or later.
****************************************************************************/

#ifndef CEFSWITCHES_H
#define CEFSWITCHES_H

namespace QCefKits
{
namespace switches
{

extern const char kMultiThreadedMessageLoop[];
extern const char kExternalMessagePump[];
extern const char kCachePath[];
extern const char kUrl[];
extern const char kOffScreenRenderingEnabled[];
extern const char kOffScreenFrameRate[];
extern const char kTransparentPaintingEnabled[];
extern const char kShowUpdateRect[];
extern const char kSharedTextureEnabled[];
extern const char kExternalBeginFrameEnabled[];
extern const char kMouseCursorChangeDisabled[];
extern const char kOffline[];
extern const char kRequestContextPerBrowser[];
extern const char kRequestContextSharedCache[];
extern const char kBackgroundColor[];
extern const char kEnableGPU[];
extern const char kFilterURL[];
extern const char kUseViews[];
extern const char kHideFrame[];
extern const char kHideControls[];
extern const char kAlwaysOnTop[];
extern const char kHideTopMenu[];
extern const char kWidevineCdmPath[];
extern const char kSslClientCertificate[];
extern const char kCRLSetsPath[];
extern const char kLoadExtension[];
extern const char kNoActivate[];
extern const char kEnableChromeRuntime[];

}  // namespace switches
}  // namespace client

#endif // CEFSWITCHES_H

/****************************************************************************
** CefSubProcess used by QCefKits Library
** Copyright (C) panuins
** Contact: pl_014@163.com
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GPLV2 or later.
****************************************************************************/

#ifndef CLIENTAPPOTHER_H
#define CLIENTAPPOTHER_H

#include "ClientApp.h"

// Client app implementation for other process types.
class ClientAppOther : public ClientApp
{
public:
    ClientAppOther();

private:
    IMPLEMENT_REFCOUNTING(ClientAppOther);
    DISALLOW_COPY_AND_ASSIGN(ClientAppOther);
};

#endif // CLIENTAPPOTHER_H

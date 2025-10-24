#pragma once

#include "itexture.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IDepthStencil
{
public:
    virtual ~IDepthStencil() = default;

    virtual void Clear() = 0;
    virtual void Release() = 0;

    virtual void* GetResource() = 0;
    virtual ITexture* GetTexture() = 0;
};
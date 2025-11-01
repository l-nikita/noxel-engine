#pragma once

#include "itexture.hpp"
#include "irenderresource.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IDepthStencil : public IRenderResource
{
public:
    virtual ~IDepthStencil() = default;

    virtual void Clear() = 0;

    virtual void* GetResource() = 0;
    virtual ITexture* GetTexture() = 0;
};
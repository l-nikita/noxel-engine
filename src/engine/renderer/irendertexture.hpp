#pragma once

#include <glm/glm.hpp>

#include "itexture.hpp"
#include "idepthstencil.hpp"
#include "irenderresource.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IRenderTexture : public IRenderResource
{
public:
    virtual ~IRenderTexture() = default;

    virtual void Clear(glm::vec4 color) = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual void Release() = 0;

    // For DirectX 11 is render target view. For OpenGL is frame buffer object.
    virtual void* GetResource() = 0;
    virtual IDepthStencil* GetDepthStencil() = 0;
    virtual ITexture* GetTexture() = 0;
};
#pragma once

#include <glm/glm.hpp>

#include "renderer/itexture.hpp"

class IRenderTarget
{
public:
    virtual ~IRenderTarget() = default;

    virtual void Clear(glm::vec4 color) = 0;
    virtual void Bind() = 0;
    virtual void Destroy() = 0;

    virtual ITexture* GetTexture() = 0;
};
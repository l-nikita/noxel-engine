#pragma once

#include <cstdint>

enum class TextureFormat 
{
    RGBA8,
    RGBA32F,
    DEPTH24STENCIL8,
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ITexture
{
public:
    virtual ~ITexture() = default;

    virtual void Bind(std::uint32_t slot = 0) = 0;
    virtual void Unbind(std::uint32_t slot = 0) = 0;
    virtual void Release() = 0;

    virtual std::uint32_t GetWidth() const = 0;
    virtual std::uint32_t GetHeight() const = 0;
    virtual TextureFormat GetFormat() const = 0;
};

extern ITexture* g_ErrorTexture;
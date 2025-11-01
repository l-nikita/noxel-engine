#pragma once

#include <cstdint>
#include "irenderresource.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
enum class TextureFormat 
{
    INVALID,
    RGBA8,
    RGBA32F,
    DEPTH24STENCIL8,
};

enum class TextureUsage
{
    DEFAULT,
    RENDER_TARGET,
    DEPTH_STENCIL,
    SHADER_RESOURCE,
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct TextureDesc
{
    std::uint32_t Width = 0;
    std::uint32_t Height = 0;
    std::uint32_t MipLevels = 1;
    TextureFormat Format = TextureFormat::RGBA8;
    TextureUsage Usage = TextureUsage::DEFAULT;
    void* Data = nullptr;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ITexture : public IRenderResource
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
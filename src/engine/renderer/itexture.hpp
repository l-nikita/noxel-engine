#pragma once

#include <cstdint>
#include <vector>
#include <string>
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
    R32_TYPELESS,
    R32_FLOAT,
    D32_FLOAT,
};

enum class TextureUsage
{
    DEFAULT,    // GPU read/write
    IMMUTABLE,  // Read-only (created once)
    DYNAMIC,    // CPU can update
    STAGING,    // Used to copy CPU <-> GPU.
};

enum class TextureBindFlags : std::uint32_t
{
    NONE = 0,
    SHADER_RESOURCE = 1 << 0,
    RENDER_TARGET = 1 << 1,
    DEPTH_STENCIL = 1 << 2,
};

inline TextureBindFlags operator|(TextureBindFlags a, TextureBindFlags b)
{
    return static_cast<TextureBindFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline bool operator&(TextureBindFlags a, TextureBindFlags b)
{
    return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct TextureDesc
{
    std::uint32_t Width = 0;
    std::uint32_t Height = 0;
    std::uint32_t MipLevels = 1;
    bool IsCubemap = 0;
    TextureFormat Format = TextureFormat::RGBA8;
    TextureFormat ShaderFormat = TextureFormat::RGBA8;
    TextureUsage Usage = TextureUsage::DEFAULT;
    TextureBindFlags BindFlags = TextureBindFlags::NONE;
    std::vector<void*> Data;
};

static const std::string CUBEMAP_FACES_ORDER[] = {
    "px", "nx", "py", "ny", "pz", "nz"
};

constexpr auto CUBEMAP_FACES_COUNT = 6;

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
    virtual bool IsCubemap() const = 0;
};

extern ITexture* g_ErrorTexture;
#pragma once

enum class TextureFormat {
    RGBA8,
    RGBA32F,
    DEPTH24STENCIL8,
};

class ITexture
{
public:
    virtual ~ITexture() = default;

    virtual void Bind(unsigned int slot = 0) = 0;
    virtual void Unbind(unsigned int slot = 0) = 0;
    virtual void Destroy() = 0;

    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;
    virtual TextureFormat GetFormat() const = 0;
};
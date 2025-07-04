#pragma once

constexpr auto MATERIAL_TEXTURES_SLOTS = 8;

class IMaterial
{
public:
    virtual ~IMaterial() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Destroy() = 0;

    // Set a texture to a slot (e.g. diffuse, normal map, etc.)
    virtual void SetTexture(unsigned int slot, class ITexture* texture) = 0;
    virtual void SetShader(class IShader* shader) = 0;

    virtual IShader* GetShader() const = 0;
    virtual ITexture* GetTexture(unsigned int slot) const = 0;
};
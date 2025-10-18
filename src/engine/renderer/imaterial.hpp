#pragma once

#include <cstdint>

constexpr auto MATERIAL_TEXTURES_SLOTS = 8;

class IMaterial
{
public:
    virtual ~IMaterial() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Release() = 0;

    // Set a texture to a slot (e.g. diffuse, normal map, etc.)
    virtual void SetTexture(std::uint32_t slot = 0, class ITexture* texture = nullptr) = 0;
    virtual void SetShader(class IShaderProgram* shader) = 0;

    virtual IShaderProgram* GetShader() const = 0;
    virtual ITexture* GetTexture(std::uint32_t slot = 0) const = 0;
};

extern IMaterial* g_ErrorMaterial;
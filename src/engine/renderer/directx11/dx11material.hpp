#pragma once

#include <cstdint>
#include <d3d11.h>
#include "renderer/imaterial.hpp"
#include "renderer/ishader.hpp"
#include "renderer/itexture.hpp"
#include "assetsystem/assets.hpp"

class DX11Material : public IMaterial 
{
public:
    DX11Material(ID3D11Device* device, ID3D11DeviceContext* context, MaterialAsset* asset);
    ~DX11Material() { Destroy(); }

    void Bind() override;
    void Unbind() override;
    void Destroy() override;

    void SetShader(IShaderProgram* shader) override;
    void SetTexture(std::uint32_t slot = 0, ITexture* texture = nullptr) override;

    IShaderProgram* GetShader() const override;
    ITexture* GetTexture(std::uint32_t slot = 0) const override;

private:
    MaterialAsset* m_asset;
    ID3D11DeviceContext* m_context;
    IShaderProgram* m_shader = nullptr;
    ITexture* m_textures[MATERIAL_TEXTURES_SLOTS] = { nullptr }; // 8 slots by default
};
#pragma once

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

    void SetShader(IShader* shader) override;
    void SetTexture(unsigned int slot, ITexture* texture) override;

    IShader* GetShader() const override;
    ITexture* GetTexture(unsigned int slot) const override;

private:
    MaterialAsset* m_asset;
    ID3D11DeviceContext* m_context;
    IShader* m_shader = nullptr;
    ITexture* m_textures[MATERIAL_TEXTURES_SLOTS] = { nullptr }; // 8 slots by default
};
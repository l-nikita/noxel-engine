#pragma once

#include "renderer/imaterial.hpp"
#include "renderer/ishader.hpp"
#include "renderer/itexture.hpp"
#include "assetsystem/assets.hpp"

struct ID3D11DeviceContext;

class D3D11Material : public IMaterial 
{
public:
    D3D11Material(ID3D11Device* device, ID3D11DeviceContext* context, MaterialAsset* asset);
    ~D3D11Material() { Destroy(); }

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
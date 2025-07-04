#pragma once

#include "renderer/itexture.hpp"
#include "assetsystem/assets.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class D3D11Texture : public ITexture 
{
public:
    D3D11Texture(ID3D11Device* device, ID3D11DeviceContext* context, TextureAsset* asset);
    ~D3D11Texture() { Destroy(); }

    void Bind(unsigned int slot = 0) override;
    void Unbind(unsigned int slot = 0) override;
    void Destroy() override;

    unsigned int GetWidth() const override { return m_width; }
    unsigned int GetHeight() const override { return m_height; }
    TextureFormat GetFormat() const override { return m_format; }

private:
    ID3D11DeviceContext* m_context = nullptr;
    ID3D11ShaderResourceView* m_srv = nullptr;
    ID3D11Resource* m_resource = nullptr;
    ID3D11Texture2D* m_texture = nullptr;

    unsigned int m_width = 0;
    unsigned int m_height = 0;
    TextureFormat m_format = TextureFormat::RGBA8;
};
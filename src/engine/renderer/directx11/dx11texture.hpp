#pragma once

#include <d3d11.h>
#include <cstdint>
#include "renderer/itexture.hpp"
#include "assetsystem/assets.hpp"

class DX11Texture : public ITexture 
{
public:
    DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, TextureAsset* asset);
    ~DX11Texture() { Destroy(); }

    void Bind(std::uint32_t slot = 0) override;
    void Unbind(std::uint32_t slot = 0) override;
    void Destroy() override;

    std::uint32_t GetWidth() const override { return m_width; }
    std::uint32_t GetHeight() const override { return m_height; }
    TextureFormat GetFormat() const override { return m_format; }

private:
    ID3D11DeviceContext* m_context = nullptr;

    ID3D11ShaderResourceView* m_srv = nullptr;
    ID3D11Resource* m_resource = nullptr;
    ID3D11Texture2D* m_texture = nullptr;
    ID3D11SamplerState* m_samplerState = nullptr;

    std::uint32_t m_width = 0;
    std::uint32_t m_height = 0;
    TextureFormat m_format = TextureFormat::RGBA8;
};
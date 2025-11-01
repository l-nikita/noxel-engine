#pragma once

#include <d3d11.h>
#include <cstdint>
#include "renderer/itexture.hpp"

class TextureAsset;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11Texture : public ITexture 
{
public:
    DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, const TextureDesc& desc);
    DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture);
    ~DX11Texture() { Release(); }

    void Bind(std::uint32_t slot = 0) override;
    void Unbind(std::uint32_t slot = 0) override;
    void Release() override;

    std::uint32_t GetWidth() const override { return m_width; }
    std::uint32_t GetHeight() const override { return m_height; }
    TextureFormat GetFormat() const override { return m_format; }

    ID3D11Texture2D* GetResource() { return m_texture; }
    ID3D11ShaderResourceView* GetSRV() { return m_srv; }

private:
    ID3D11ShaderResourceView* m_srv = nullptr;
    ID3D11Texture2D* m_texture = nullptr;

    std::uint32_t m_width = 0;
    std::uint32_t m_height = 0;
    TextureFormat m_format = TextureFormat::INVALID;

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
};
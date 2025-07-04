#include <stdexcept>
#include <d3d11.h>
#include "d3d11texture.hpp"
#include "assetsystem/assets.hpp"

D3D11Texture::D3D11Texture(ID3D11Device* device,  ID3D11DeviceContext* context, TextureAsset* asset) 
    : m_context(context)
{
    TextureData data = asset->Load();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = data.Width;
    desc.Height = data.Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Use correct DXGI_FORMAT
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data.Image;
    initData.SysMemPitch = data.Width * 4;

    HRESULT hr = device->CreateTexture2D(&desc, &initData, &m_texture);
    if (FAILED(hr)) 
        throw std::runtime_error("Failed to create texture2D");

    hr = device->CreateShaderResourceView(m_texture, nullptr, &m_srv);
    if (FAILED(hr)) 
    {
        m_texture->Release();
        throw std::runtime_error("Failed to create shader resource view");
    }

    m_width = desc.Width;
    m_height = desc.Height;
}


void D3D11Texture::Destroy()
{
    if (m_srv) m_srv->Release();
    if (m_texture) m_texture->Release();
}

void D3D11Texture::Bind(unsigned int slot)
{
    m_context->PSSetShaderResources(slot, 1, &m_srv);
}

void D3D11Texture::Unbind(unsigned int slot)
{
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_context->PSSetShaderResources(slot, 1, nullSRV);
}

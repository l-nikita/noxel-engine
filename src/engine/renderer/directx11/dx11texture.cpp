#include <stdexcept>
#include <d3d11.h>
#include "dx11texture.hpp"
#include "assetsystem/assets.hpp"

DX11Texture::DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, TextureAsset* asset)
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
        FATAL_ERROR("DX11: Failed to create Texture2D.", CMD::RENDERING);

    hr = device->CreateShaderResourceView(m_texture, nullptr, &m_srv);
    if (FAILED(hr)) 
    {
        m_texture->Release();
        FATAL_ERROR("DX11: Failed to create shader resource view.", CMD::RENDERING);
    }

    m_width = desc.Width;
    m_height = desc.Height;

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
}

DX11Texture::DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture)
    : m_context(context), m_texture(texture)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    HRESULT hr = device->CreateShaderResourceView(m_texture, &srvDesc, &m_srv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create shader resource for Texture2D.", CMD::RENDERING);

    D3D11_TEXTURE2D_DESC desc;
    m_texture->GetDesc(&desc);
    m_width = desc.Width;
    m_height = desc.Height;
}

void DX11Texture::Destroy()
{
    if (m_srv) m_srv->Release();
    if (m_texture) m_texture->Release();
}

void DX11Texture::Bind(std::uint32_t slot)
{
    m_context->PSSetShaderResources(slot, 1, &m_srv);
}

void DX11Texture::Unbind(std::uint32_t slot)
{
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_context->PSSetShaderResources(slot, 1, nullSRV);
}

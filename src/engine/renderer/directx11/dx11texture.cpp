#include <stdexcept>
#include <string>
#include <d3d11.h>
#include "dx11texture.hpp"
#include "assetsystem/assets.hpp"

DX11Texture::DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, TextureAsset* asset)
    : m_context(context)
{
    assert(asset);

    TextureData* data = asset->Load();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = data->Width;
    desc.Height = data->Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Use correct DXGI_FORMAT
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data->Image;
    initData.SysMemPitch = data->Width * 4;

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

    //D3D11_SAMPLER_DESC sampDesc = {};
    //sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    //sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    //sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    //sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    delete data;
}

DX11Texture::DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture)
    : m_context(context), m_texture(texture)
{
    assert(texture);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    D3D11_TEXTURE2D_DESC desc;
    m_texture->GetDesc(&desc);

    if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
    {
        HRESULT hr = device->CreateShaderResourceView(m_texture, &srvDesc, &m_srv);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create shader resource for Texture2D.", CMD::RENDERING);
    }

    m_width = desc.Width;
    m_height = desc.Height;
}

// TODO: Make seperate class for render texture
DX11Texture::DX11Texture(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height, TextureFormat format)
    : m_context(context)
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = ToDXGIFormat(format);
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;

    if (format == TextureFormat::DEPTH24STENCIL8)
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    else
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&desc, nullptr, &m_texture);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Texture2D (Render texture).", CMD::RENDERING);

    if (format != TextureFormat::DEPTH24STENCIL8)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        device->CreateShaderResourceView(m_texture, &srvDesc, &m_srv);
    }
}

void DX11Texture::Release()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }

    if (m_texture)
    {
        m_texture->Release();
        m_texture = nullptr;
    }
}

void DX11Texture::Bind(std::uint32_t slot)
{
    assert(m_srv);
    m_context->PSSetShaderResources(slot, 1, &m_srv);
}

void DX11Texture::Unbind(std::uint32_t slot)
{
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_context->PSSetShaderResources(slot, 1, nullSRV);
}

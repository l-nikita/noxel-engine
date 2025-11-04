#pragma once

#include <d3d11.h>
#include "dbg/debug.hpp"
#include "renderer/itexture.hpp"

namespace DX11Helper
{
    static DXGI_FORMAT ToDXGIFormat(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGBA8:              return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::RGBA32F:            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case TextureFormat::DEPTH24STENCIL8:    return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case TextureFormat::R32_TYPELESS:       return DXGI_FORMAT_R32_TYPELESS;
        case TextureFormat::R32_FLOAT:          return DXGI_FORMAT_R32_FLOAT;
        case TextureFormat::D32_FLOAT:          return DXGI_FORMAT_D32_FLOAT;
        default:                                return DXGI_FORMAT_UNKNOWN;
        }
    }

    static TextureFormat ToTextureFormat(DXGI_FORMAT dxgiFormat)
    {
        switch (dxgiFormat)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:       return TextureFormat::RGBA8;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:   return TextureFormat::RGBA32F;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:    return TextureFormat::DEPTH24STENCIL8;
        case DXGI_FORMAT_R32_TYPELESS:         return TextureFormat::R32_TYPELESS;
        case DXGI_FORMAT_R32_FLOAT:            return TextureFormat::R32_FLOAT;
        case DXGI_FORMAT_D32_FLOAT:            return TextureFormat::D32_FLOAT;
        default:                               return TextureFormat::INVALID;
        }
    }

    static D3D11_TEXTURE2D_DESC CreateTexture2DDesc(const TextureDesc& desc)
    {
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = desc.Width;
        texDesc.Height = desc.Height;
        texDesc.MipLevels = desc.MipLevels;
        texDesc.ArraySize = std::max<std::uint32_t>(1, desc.Data.size());
        texDesc.Format = ToDXGIFormat(desc.Format);
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.MiscFlags = desc.IsCubemap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
        texDesc.BindFlags = 0;
        texDesc.CPUAccessFlags = 0;

        switch (desc.Usage)
        {
        case TextureUsage::DEFAULT:
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            break;
        case TextureUsage::IMMUTABLE:
            texDesc.Usage = D3D11_USAGE_IMMUTABLE;
            break;
        case TextureUsage::DYNAMIC:
            texDesc.Usage = D3D11_USAGE_DYNAMIC;
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            break;
        case TextureUsage::STAGING:
            texDesc.Usage = D3D11_USAGE_STAGING;
            texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            break;
        }

        if (desc.BindFlags & TextureBindFlags::SHADER_RESOURCE)
            texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if (desc.BindFlags & TextureBindFlags::RENDER_TARGET)
            texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        if (desc.BindFlags & TextureBindFlags::DEPTH_STENCIL)
            texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

        return texDesc;
    }

    static void CreateTexture2D(
        ID3D11Device* device, 
        const D3D11_TEXTURE2D_DESC* desc, 
        const D3D11_SUBRESOURCE_DATA* data, 
        ID3D11Texture2D** texture)
    {
        HRESULT hr = device->CreateTexture2D(desc, data, texture);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create Texture2D.", CMD::RENDERING);
    }

    static void CreateShaderResourceViewForTexture2D(
        ID3D11Device* device, 
        ID3D11Texture2D* texture, 
        ID3D11ShaderResourceView** srv,
        DXGI_FORMAT format,
        D3D11_SRV_DIMENSION srvDimension = D3D11_SRV_DIMENSION_TEXTURE2D)
    {
        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);

        if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = format;
            srvDesc.ViewDimension = srvDimension;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;

            HRESULT hr = device->CreateShaderResourceView(texture, &srvDesc, srv);
            if (FAILED(hr))
                FATAL_ERROR("DX11: Failed to create shader resource for Texture2D.", CMD::RENDERING);
        }
    }

    static void CreateDepthStencilView(
        ID3D11Device* device, 
        ID3D11Resource* resource, 
        D3D11_DEPTH_STENCIL_VIEW_DESC* desc, 
        ID3D11DepthStencilView** dsv)
    {
        HRESULT hr = device->CreateDepthStencilView(resource, desc, dsv);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create Depth Stencil View.", CMD::RENDERING);
    }

    static void CreateRenderTargetView(
        ID3D11Device* device, 
        ID3D11Resource* resource, 
        D3D11_RENDER_TARGET_VIEW_DESC* desc,
        ID3D11RenderTargetView** rtv)
    {
        HRESULT hr = device->CreateRenderTargetView(resource, desc, rtv);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create Render Target View.", CMD::RENDERING);
    }
}
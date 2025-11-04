#include "dx11depthstencil.hpp"
#include "dx11rendertexture.hpp"
#include "dx11texture.hpp"
#include "dx11helper.hpp"
#include "renderer/irenderer.hpp"
#include "renderer/irenderfactory.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DX11DepthStencil::DX11DepthStencil(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height, bool bindable)
    : m_device(device), m_context(context)
{
    auto factory = g_Renderer->GetFactory();

    TextureDesc desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;

    if (!bindable)
    {
        desc.Format = TextureFormat::DEPTH24STENCIL8;
        desc.BindFlags = TextureBindFlags::DEPTH_STENCIL;

        m_texture = factory->CreateTexture(desc);
        auto res = static_cast<DX11Texture*>(m_texture)->GetResource();

        DX11Helper::CreateDepthStencilView(m_device, res, nullptr, &m_dsv);
    }
    else
    {
        desc.Format = TextureFormat::R32_TYPELESS;
        desc.ShaderFormat = TextureFormat::R32_FLOAT;
        desc.BindFlags = TextureBindFlags::DEPTH_STENCIL | TextureBindFlags::SHADER_RESOURCE;

        m_texture = factory->CreateTexture(desc);
        auto res = static_cast<DX11Texture*>(m_texture)->GetResource();

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        DX11Helper::CreateDepthStencilView(m_device, res, &dsvDesc, &m_dsv);
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11DepthStencil::Clear()
{
    m_context->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11DepthStencil::Release()
{
    if (m_dsv)
    {
        m_dsv->Release();
        m_dsv = nullptr;
    }

    if (m_texture)
    {
        g_Renderer->GetFactory()->Release(m_texture);
        m_texture = nullptr;
    }
}
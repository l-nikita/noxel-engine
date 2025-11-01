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
DX11DepthStencil::DX11DepthStencil(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height)
    : m_device(device), m_context(context)
{
    TextureDesc desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = TextureFormat::DEPTH24STENCIL8;
    desc.Usage = TextureUsage::DEPTH_STENCIL;
    desc.MipLevels = 1;

    m_texture = g_Renderer->GetFactory()->CreateTexture(desc);
    auto res = static_cast<DX11Texture*>(m_texture)->GetResource();

    DX11Helper::CreateDepthStencilView(m_device, res, nullptr, &m_dsv);
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
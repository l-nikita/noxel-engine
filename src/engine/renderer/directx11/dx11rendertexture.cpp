#include "dx11renderer.hpp"
#include "dx11rendertexture.hpp"
#include "dx11texture.hpp"
#include "dx11depthstencil.hpp"
#include "dx11factory.hpp"
#include "dx11helper.hpp"
#include "dbg/debug.hpp"

DX11RenderTexture::DX11RenderTexture(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height, bool depth, TextureFormat format)
    : m_device(device), m_context(context)
{
    TextureDesc desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = format;
    desc.ShaderFormat = format;
    desc.BindFlags = TextureBindFlags::RENDER_TARGET | TextureBindFlags::SHADER_RESOURCE;
    desc.MipLevels = 1;

    m_texture = g_Renderer->GetFactory()->CreateTexture(desc);
    auto res = static_cast<DX11Texture*>(m_texture)->GetResource();

    m_fromFactory = true;

    DX11Helper::CreateRenderTargetView(device, res, nullptr, &m_rtv);

    if (depth)
        m_depthStencil = g_Renderer->GetFactory()->CreateDepthStencil(width, height, false);
}

DX11RenderTexture::DX11RenderTexture(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture, bool depth, TextureFormat format)
    : m_device(device), m_context(context)
{
    // TODO: make from factory
    m_texture = new DX11Texture(m_device, m_context, texture);

    auto res = static_cast<DX11Texture*>(m_texture)->GetResource();
    DX11Helper::CreateRenderTargetView(device, res, nullptr, &m_rtv);

    if (depth)
        m_depthStencil = g_Renderer->GetFactory()->CreateDepthStencil(m_texture->GetWidth(), m_texture->GetHeight(), false);
}

void DX11RenderTexture::Clear(glm::vec4 color)
{
    float clearColor[4] = { color.r, color.g, color.b, color.a };
    m_context->ClearRenderTargetView(m_rtv, clearColor);

    if (m_depthStencil)
        m_depthStencil->Clear();
}

void DX11RenderTexture::Bind()
{
    if (m_depthStencil)
        m_context->OMSetRenderTargets(1, &m_rtv, static_cast<ID3D11DepthStencilView*>(m_depthStencil->GetResource()));
    else
        m_context->OMSetRenderTargets(1, &m_rtv, nullptr);
}

void DX11RenderTexture::Unbind()
{
    m_context->OMSetRenderTargets(0, nullptr, nullptr);
}

void DX11RenderTexture::Release()
{
    if (IsReleased())
        return;

    m_isReleased = true;

    if (m_rtv)
        m_rtv->Release(), m_rtv = nullptr;

    if (m_texture)
    {
        if (m_fromFactory)
            g_Renderer->GetFactory()->Release(m_texture);
        else
            delete m_texture;

        m_texture = nullptr;
    }

    if (m_depthStencil)
        g_Renderer->GetFactory()->Release(m_depthStencil), m_depthStencil = nullptr;
}
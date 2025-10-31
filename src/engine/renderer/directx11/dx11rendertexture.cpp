#include "dx11renderer.hpp"
#include "dx11rendertexture.hpp"
#include "dx11texture.hpp"
#include "dx11depthstencil.hpp"
#include "dx11factory.hpp"
#include "dbg/debug.hpp"

DX11RenderTexture::DX11RenderTexture(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height, bool depth, TextureFormat format)
    : m_device(device), m_context(context)
{
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = ToDXGIFormat(format);
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // важно
    texDesc.CPUAccessFlags = 0;

    ID3D11Texture2D* renderTex = nullptr;
    HRESULT hr = m_device->CreateTexture2D(&texDesc, nullptr, &renderTex);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Texture2D for render target.", CMD::RENDERING);

    hr = m_device->CreateRenderTargetView(renderTex, nullptr, &m_rtv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create render target.", CMD::RENDERING);

    m_texture = new DX11Texture(m_device, m_context, renderTex, format);

    if (depth)
        m_depthStencil = g_Renderer->GetFactory()->CreateDepthStencil(this);
}

DX11RenderTexture::DX11RenderTexture(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture, bool depth, TextureFormat format)
    : m_device(device), m_context(context)
{
    HRESULT hr = m_device->CreateRenderTargetView(texture, nullptr, &m_rtv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create render target from Texture2D.", CMD::RENDERING);

    m_texture = new DX11Texture(m_device, m_context, texture, format);

    if (depth)
        m_depthStencil = g_Renderer->GetFactory()->CreateDepthStencil(this);
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
    if (m_texture)
    {
        delete m_texture;
        m_texture = nullptr;
    }

    if (m_rtv)
    {
        m_rtv->Release();
        m_rtv = nullptr;
    }

    if (m_depthStencil)
    {
        delete m_depthStencil;
        m_depthStencil = nullptr;
    }
}
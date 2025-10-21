#include "dx11renderer.hpp"
#include "dx11rendertarget.hpp"
#include "dx11texture.hpp"
#include "dbg/debug.hpp"

DX11RenderTarget::DX11RenderTarget(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height)
    : m_device(device), m_context(context)
{
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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

    m_texture = new DX11Texture(m_device, m_context, renderTex);

    // Depth Stencil
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthTex = nullptr;
    hr = m_device->CreateTexture2D(&depthDesc, nullptr, &depthTex);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil Texture.", CMD::RENDERING);

    hr = m_device->CreateDepthStencilView(depthTex, nullptr, &m_dsv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil View.", CMD::RENDERING);

    depthTex->Release();
}

DX11RenderTarget::DX11RenderTarget(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture)
    : m_device(device), m_context(context)
{
    HRESULT hr = m_device->CreateRenderTargetView(texture, nullptr, &m_rtv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create render target from Texture2D.", CMD::RENDERING);

    m_texture = new DX11Texture(m_device, m_context, texture);

    D3D11_TEXTURE2D_DESC texDesc = {};
    texture->GetDesc(&texDesc);

    // Depth Stencil
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = texDesc.Width;
    depthDesc.Height = texDesc.Height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthTex = nullptr;
    hr = m_device->CreateTexture2D(&depthDesc, nullptr, &depthTex);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil Texture.", CMD::RENDERING);

    hr = m_device->CreateDepthStencilView(depthTex, nullptr, &m_dsv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil View.", CMD::RENDERING);

    depthTex->Release();
}

void DX11RenderTarget::Clear(glm::vec4 color)
{
    float clearColor[4] = { color.r, color.g, color.b, color.a };
    m_context->ClearRenderTargetView(m_rtv, clearColor);
    m_context->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11RenderTarget::Bind()
{
    m_context->OMSetRenderTargets(1, &m_rtv, m_dsv);
}

void DX11RenderTarget::Release()
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

    if (m_dsv)
    {
        m_dsv->Release();
        m_dsv = nullptr;
    }
}
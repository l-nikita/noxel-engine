#include "dx11depthstencil.hpp"
#include "dx11rendertexture.hpp"
#include "dx11texture.hpp"
#include "dbg/debug.hpp"

DX11DepthStencil::DX11DepthStencil(ID3D11Device* device, ID3D11DeviceContext* context, IRenderTexture* rt)
    : m_device(device), m_context(context)
{
    auto tex = rt->GetTexture();

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = tex->GetWidth();
    depthDesc.Height = tex->GetHeight();
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthTex = nullptr;
    HRESULT hr = m_device->CreateTexture2D(&depthDesc, nullptr, &depthTex);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil Texture.", CMD::RENDERING);

    hr = m_device->CreateDepthStencilView(depthTex, nullptr, &m_dsv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil View.", CMD::RENDERING);

    m_texture = new DX11Texture(m_device, m_context, depthTex);
}

DX11DepthStencil::DX11DepthStencil(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height)
    : m_device(device), m_context(context)
{
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
    HRESULT hr = m_device->CreateTexture2D(&depthDesc, nullptr, &depthTex);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil Texture.", CMD::RENDERING);

    hr = m_device->CreateDepthStencilView(depthTex, nullptr, &m_dsv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create Depth Stencil View.", CMD::RENDERING);

    m_texture = new DX11Texture(m_device, m_context, depthTex);
}

void DX11DepthStencil::Clear()
{
    m_context->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11DepthStencil::Release()
{
    if (m_texture)
    {
        delete m_texture;
        m_texture = nullptr;
    }

    if (m_dsv)
    {
        m_dsv->Release();
        m_dsv = nullptr;
    }
}
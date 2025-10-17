#include <SDL3/SDL.h>
#include <glm/gtc/type_ptr.hpp>
#include "dx11renderer.hpp"
#include "dx11material.hpp"
#include "dx11mesh.hpp"
#include "dx11texture.hpp"
#include "dx11rendertarget.hpp"
#include "iengine.hpp"
#include "window.hpp"
#include "renderer/imaterial.hpp"
#include "renderer/model.hpp"
#include "renderer/render_camera.hpp"
#include "renderer/vertex.hpp"
#include "gui/igui.hpp"
#include "dbg/cmd/cmd.hpp"

// Shaders should be included here (somewhere). Compiler issue.
#include "shaders/dx11shader_default.hpp"
#include "shaders/dx11shader_gizmos.hpp"

#define INITGUID
#include <d3d11.h>
#include <dxgi1_2.h>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::CreateDevice()
{
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // adapter
        D3D_DRIVER_TYPE_HARDWARE,   // driver type
        nullptr,                    // software
        0,                          // flags
        nullptr,                    // feature levels
        0,                          // feature levels
        D3D11_SDK_VERSION,          // version
        &m_device,                  // device
        &featureLevel,              // feature level
        &m_context);                // device context

    if (FAILED(hr))
        FATAL_ERROR("DX11: D3D11CreateDevice failed", CMD::RENDERING);
}

void DX11Renderer::CreateSwapChain()
{
    assert(m_device);

    void* ptr = g_Engine->GetWindow()->GetPointer();
    if (!ptr)
        FATAL_ERROR("DX11: Couldn't get window pointer from SDL window!", CMD::RENDERING);

    HWND hwnd = (HWND)ptr;

    int width, height;
    g_Engine->GetWindow()->GetSize(&width, &height);

    m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgiDevice);
    m_dxgiDevice->GetAdapter(&m_dxgiAdapter);
    m_dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&m_dxgiFactory);

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.Width = width;
    scDesc.Height = height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.Stereo = FALSE;
    scDesc.SampleDesc.Count = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = 2;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(m_device, hwnd, &scDesc, nullptr, nullptr, &m_swapChain);

    if (FAILED(hr))
        FATAL_ERROR("DX11: CreateSwapChainForHwnd failed", CMD::RENDERING);
}

void DX11Renderer::Init()
{
    CreateDevice();
    CreateSwapChain();

    HRESULT hr = m_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&m_backBuffer);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to get back buffer.", CMD::RENDERING);

    //-------------------------------------------
    int width, height;
    g_Engine->GetWindow()->GetSize(&width, &height);

    OnWindowResize(width, height);

    //-------------------------------------------
    m_factory = new DX11Factory(m_device, m_context);
    m_camera = new RenderCamera(width, height);

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;
    m_context->RSSetViewports(1, &viewport);

	CMD_INFO("DX11: Renderer initialized", CMD::RENDERING);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::CopyToBackBuffer(IRenderTarget* rt)
{
    auto tex = static_cast<DX11Texture*>(rt->GetTexture());
    m_context->CopyResource(GetBackBuffer(), tex->GetTexture2D());
}

void DX11Renderer::Swap()
{
    m_swapChain->Present(m_vsyncEnabled ? 1 : 0, m_vsyncEnabled ? 0 : DXGI_PRESENT_ALLOW_TEARING);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::SetPrimitiveTopology(PrimitiveTopology topology)
{
    if (topology == LINES)
        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    else if (topology == TRIANGLES)
        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::Shutdown()
{
	delete m_camera;
    m_swapChain->Release();
    m_context->Release();
    m_device->Release();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::OnWindowResize(int width, int height)
{
}
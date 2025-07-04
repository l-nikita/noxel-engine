#include <SDL3/SDL.h>
#include "d3d11renderer.hpp"
#include "iengine.hpp"
#include "cmd/cmd.hpp"
#include <d3d11.h>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool D3D11Renderer::Init()
{
	int width, height;
    auto window = g_Engine->GetWindow();

	SDL_GetWindowSizeInPixels(window, &width, &height);

    void* ptr = g_Engine->GetNativeWindowHandle();
    if (!ptr) 
    {
        std::cerr << "Couldn't get window handle from SDL window!\n";
        return false;
    }

    HWND hwnd = (HWND)ptr;

    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.BufferCount = 1;
    scDesc.BufferDesc.Width = width;
    scDesc.BufferDesc.Height = height;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = hwnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.Windowed = true;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &scDesc, &m_swapChain, &m_device, nullptr, &m_context
    );

    if (FAILED(hr)) 
    {
        std::cerr << "D3D11CreateDeviceAndSwapChain failed\n";
        return false;
    }

    OnWindowResize(width, height);

	CMD_LOG("] Renderer initialized", INFO);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void D3D11Renderer::Render()
{
    float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

    m_context->OMSetRenderTargets(1, &m_rtv, nullptr);
    m_context->ClearRenderTargetView(m_rtv, clearColor);

    // Тут рисуем что нужно

    m_swapChain->Present(1, 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void D3D11Renderer::Shutdown()
{
	delete m_camera;
    m_rtv->Release();
    m_swapChain->Release();
    m_context->Release();
    m_device->Release();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void D3D11Renderer::OnWindowResize(int width, int height)
{
    if (m_rtv)
    {
        m_rtv->Release();
        m_rtv = nullptr;
    }

    m_width = width;
    m_height = height;

    HRESULT hr = m_swapChain->ResizeBuffers(0, m_width, m_height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr))
        throw std::runtime_error("Failed to resize swap chain buffers");

    ID3D11Texture2D* backBuffer = nullptr;
    hr = m_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
    if (FAILED(hr))
        throw std::runtime_error("Failed to get back buffer after resize");

    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_rtv);
    backBuffer->Release();
    if (FAILED(hr))
        throw std::runtime_error("Failed to create render target view after resize");

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_width);
    viewport.Height = static_cast<float>(m_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    m_context->RSSetViewports(1, &viewport);
}
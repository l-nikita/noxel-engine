#include <SDL3/SDL.h>
#include <glm/gtc/type_ptr.hpp>
#include "dx11renderer.hpp"
#include "dx11material.hpp"
#include "dx11mesh.hpp"
#include "dx11texture.hpp"
#include "iengine.hpp"
#include "renderer/imaterial.hpp"
#include "renderer/model.hpp"
#include "renderer/render_camera.hpp"
#include "renderer/vertex.hpp"
#include "renderer/gizmos.hpp"
#include "gui/igui.hpp"
#include "dbg/cmd/cmd.hpp"

// Shaders should be included here (somewhere). Compiler issue.
#include "shaders/dx11shader_default.hpp"
#include "shaders/dx11shader_gizmos.hpp"

#define INITGUID
#include <d3d11.h>
#include <dxgi1_2.h>

ITexture* g_ErrorTexture = nullptr;
IMaterial* g_ErrorMaterial = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool DX11Renderer::Init()
{
	int width, height;
    auto window = g_Engine->GetWindow();

	SDL_GetWindowSizeInPixels(window, &width, &height);

    void* ptr = g_Engine->GetNativeWindowHandle();
    if (!ptr) 
        FATAL_ERROR("DX11: Couldn't get window handle from SDL window!", CMD::RENDERING);

    HWND hwnd = (HWND)ptr;

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &m_device, &featureLevel, &m_context);

    if (FAILED(hr)) 
        FATAL_ERROR("DX11: D3D11CreateDevice failed", CMD::RENDERING);

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

    hr = m_dxgiFactory->CreateSwapChainForHwnd(m_device, hwnd, &scDesc, nullptr, nullptr, &m_swapChain);

    if (FAILED(hr))
        FATAL_ERROR("DX11: CreateSwapChainForHwnd failed", CMD::RENDERING);

    //-------------------------------------------
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // или DXGI_FORMAT_D32_FLOAT
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = m_device->CreateTexture2D(&depthDesc, nullptr, &m_depthStencil);
    if (FAILED(hr)) 
        FATAL_ERROR("DX11: Failed to create depth stencil buffer", CMD::RENDERING);

    hr = m_device->CreateDepthStencilView(m_depthStencil, nullptr, &m_dsv);
    if (FAILED(hr)) 
        FATAL_ERROR("DX11: Failed to create depth stencil view", CMD::RENDERING);

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_device->CreateBlendState(&blendDesc, &m_blendState);

    OnWindowResize(width, height);

    //-------------------------------------------
    m_factory = new DX11Factory(m_device, m_context);
    m_camera = new RenderCamera(width, height);

    //-------------------------------------------
    g_ErrorTexture = new DX11Texture(m_device, m_context, TextureAsset::GetError());
    g_ErrorMaterial = m_factory->CreateMaterial(MaterialAsset::GetError());

    std::vector<Vertex> cubeVertices
    {
        // Front
        {{-1, -1,  1}, {1,1,1,1}, {0,1}},
        {{ 1, -1,  1}, {1,1,1,1}, {1,1}},
        {{ 1,  1,  1}, {1,1,1,1}, {1,0}},
        {{-1,  1,  1}, {1,1,1,1}, {0,0}},

        // Back
        {{ 1, -1, -1}, {1,1,1,1}, {0,1}},
        {{-1, -1, -1}, {1,1,1,1}, {1,1}},
        {{-1,  1, -1}, {1,1,1,1}, {1,0}},
        {{ 1,  1, -1}, {1,1,1,1}, {0,0}},

        // Top
        {{-1,  1,  1}, {1,1,1,1}, {0,1}},
        {{ 1,  1,  1}, {1,1,1,1}, {1,1}},
        {{ 1,  1, -1}, {1,1,1,1}, {1,0}},
        {{-1,  1, -1}, {1,1,1,1}, {0,0}},

        // Bottom
        {{-1, -1, -1}, {1,1,1,1}, {0,1}},
        {{ 1, -1, -1}, {1,1,1,1}, {1,1}},
        {{ 1, -1,  1}, {1,1,1,1}, {1,0}},
        {{-1, -1,  1}, {1,1,1,1}, {0,0}},

        // Right
        {{ 1, -1,  1}, {1,1,1,1}, {0,1}},
        {{ 1, -1, -1}, {1,1,1,1}, {1,1}},
        {{ 1,  1, -1}, {1,1,1,1}, {1,0}},
        {{ 1,  1,  1}, {1,1,1,1}, {0,0}},

        // Left
        {{-1, -1, -1}, {1,1,1,1}, {0,1}},
        {{-1, -1,  1}, {1,1,1,1}, {1,1}},
        {{-1,  1,  1}, {1,1,1,1}, {1,0}},
        {{-1,  1, -1}, {1,1,1,1}, {0,0}},
    };

    std::vector<std::uint32_t> cubeIndices
    {
        // Front
        0, 1, 2,  2, 3, 0,
        // Back
        4, 5, 6,  6, 7, 4,
        // Top
        8, 9,10, 10,11, 8,
        // Bottom
        12,13,14, 14,15,12,
        // Right
        16,17,18, 18,19,16,
        // Left
        20,21,22, 22,23,20
    };

    // Memory leak, make resource manager
    auto* mat = m_factory->CreateMaterial("dirt");
    auto* mesh = m_factory->CreateMesh(MeshData{ cubeVertices, cubeIndices });

    /*
    for (size_t i = 0; i < 45; i++)
    {
        for (size_t j = 0; j < 45; j++)
        {
            if (mat && mesh)
            {
                auto model = new Model(mesh, mat);
                model->m_Transform.Position = glm::vec3(j*3, 0, i * 3);
                m_Models.push_back(model);
            }
        }
    }
    */

    auto model = new Model(mesh, mat);
    model->m_Transform.Position = glm::vec3(0, 0, 5);
    m_Models.push_back(model);

	CMD_INFO("] DX11: Renderer initialized", CMD::RENDERING);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::Render()
{
    float clearColor[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

    float blendFactor[4] = { 0,0,0,0 };
    m_context->OMSetBlendState(m_blendState, blendFactor, 0xFFFFFFFF);

    m_context->OMSetRenderTargets(1, &m_rtv, m_dsv);
    m_context->ClearRenderTargetView(m_rtv, clearColor);
    m_context->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

    //-------------------------------------------
    for (auto model : m_Models)
    {
        model->Draw();
    }

    static float angle = 0.0f;
    static glm::vec3 pos = glm::vec3(0);

    angle += g_Engine->DeltaTime() * 1.0f;
    glm::quat rotation = glm::angleAxis(angle, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

    pos.y = 0.3f * glm::sin(g_Engine->ElapsedTime() * 1.0f);

    Gizmos::Line(pos, glm::vec3(0, 0, 5), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    Gizmos::Box(pos, rotation, glm::vec3(1, 1, 1), glm::vec3(-1, -1, -1), glm::vec4(1, 0, 1, 0.1f));
    //Gizmos::Triangle(
    //    glm::vec3(0, 0, 0),
    //    glm::vec3(0, 0, 0),
    //    glm::vec3(1, 0, 0),
    //    glm::vec3(0, 1, 0),
    //    glm::vec4(0, 0, 1, 1)
    //);

    //-------------------------------------------
    g_Gui->Render();

    m_swapChain->Present(m_vsyncEnabled ? 1 : 0,
        m_vsyncEnabled ? 0 : DXGI_PRESENT_ALLOW_TEARING);

#ifdef _DEBUG
    m_DrawCallCount = 0;
#endif
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
    for (auto model : m_Models)
    {
        delete model;
        model = nullptr;
    }

	delete m_camera;
    m_rtv->Release();
    m_swapChain->Release();
    m_context->Release();
    m_device->Release();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::OnWindowResize(int width, int height)
{
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

    if (m_depthStencil) 
    {
        m_depthStencil->Release();
        m_depthStencil = nullptr;
    }

    m_width = width;
    m_height = height;

    HRESULT hr = m_swapChain->ResizeBuffers(2, m_width, m_height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to resize swap chain buffers", CMD::RENDERING);

    ID3D11Texture2D* backBuffer = nullptr;
    hr = m_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to get back buffer after resize", CMD::RENDERING);

    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_rtv);
    backBuffer->Release();
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create render target view after resize", CMD::RENDERING);

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = m_width;
    depthDesc.Height = m_height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;

    hr = m_device->CreateTexture2D(&depthDesc, nullptr, &m_depthStencil);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create depth stencil texture", CMD::RENDERING);

    hr = m_device->CreateDepthStencilView(m_depthStencil, nullptr, &m_dsv);
    if (FAILED(hr))
        FATAL_ERROR("DX11: Failed to create depth stencil view", CMD::RENDERING);

    m_context->OMSetRenderTargets(1, &m_rtv, m_dsv);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_width);
    viewport.Height = static_cast<float>(m_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    m_context->RSSetViewports(1, &viewport);
}
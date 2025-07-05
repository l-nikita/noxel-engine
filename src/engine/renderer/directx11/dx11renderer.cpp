#include <SDL3/SDL.h>
#include "dx11renderer.hpp"
#include "dx11material.hpp"
#include "dx11mesh.hpp"
#include "iengine.hpp"
#include "renderer/imaterial.hpp"
#include "renderer/model.hpp"
#include "renderer/render_camera.hpp"
#include "renderer/vertex.hpp"
#include "gui/igui.hpp"
#include "cmd/cmd.hpp"

#define INITGUID
#include <d3d11.h>
#include <DirectXMath.h>

static Model* TestModel;
static Model* TestModel2;
static ID3D11Buffer* ConstantBuffer;

struct MatricesCB
{
    DirectX::XMMATRIX model;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

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
        throw std::runtime_error("D3D11CreateDeviceAndSwapChain failed");
        return false;
    }

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
        throw std::runtime_error("Failed to create depth stencil buffer");

    hr = m_device->CreateDepthStencilView(m_depthStencil, nullptr, &m_dsv);
    if (FAILED(hr)) 
        throw std::runtime_error("Failed to create depth stencil view");

    OnWindowResize(width, height);

    m_camera = new RenderCamera(width, height);
    g_ErrorMaterial = new DX11Material(m_device, m_context, MaterialAsset::GetError().get());

    Vertex cubeVertices[] = {
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

    UINT cubeIndices[] = {
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

    auto mesh = new DX11Mesh(
        m_device,
        m_context,
        cubeVertices,
        sizeof(Vertex),
        static_cast<uint32_t>(std::size(cubeVertices)),
        cubeIndices,
        static_cast<uint32_t>(std::size(cubeIndices))
    );

    auto mat = new DX11Material(m_device, m_context, MaterialAsset::Get("dirt").get());
    TestModel = new Model(mesh, mat);
    TestModel2 = new Model(mesh, mat);
    TestModel2->m_Transform.Position = glm::vec3(0, 2, 5);

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(MatricesCB);
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = 0;

    m_device->CreateBuffer(&cbDesc, nullptr, &ConstantBuffer);

	CMD_LOG("] Renderer initialized", INFO);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::Render()
{
    float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

    m_context->OMSetRenderTargets(1, &m_rtv, m_dsv);
    m_context->ClearRenderTargetView(m_rtv, clearColor);
    m_context->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

    //-------------------------------------------
    glm::mat4 model = TestModel->GetTransformMatrix();
    glm::mat4 view = m_camera->GetViewMatrix();
    glm::mat4 projection = m_camera->GetProjectionMatrix();

    //MatricesCB matricesCB{ model, view, projection };

    DirectX::XMMATRIX dxModel = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&model));
    DirectX::XMMATRIX dxView = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&view));
    DirectX::XMMATRIX dxProj = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&projection));

    MatricesCB matricesCB{ DirectX::XMMatrixTranspose(dxModel), DirectX::XMMatrixTranspose(dxView), DirectX::XMMatrixTranspose(dxProj) };

    m_context->UpdateSubresource(ConstantBuffer, 0, nullptr, &matricesCB, 0, 0);
    m_context->VSSetConstantBuffers(0, 1, &ConstantBuffer);

    TestModel->Draw();


    //modelT = TestModel2->GetTransformMatrix();
    //viewT = m_camera->GetViewMatrix();
    //projectionT = m_camera->GetProjectionMatrix();

    //matricesCB = { modelT, viewT, projectionT };

    //m_context->UpdateSubresource(ConstantBuffer, 0, nullptr, &matricesCB, 0, 0);
    //m_context->VSSetConstantBuffers(0, 1, &ConstantBuffer);

    //TestModel2->Draw();

    //-------------------------------------------
    //g_Gui->Render();

    m_swapChain->Present(m_vsyncEnabled ? 1 : 0, 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::Shutdown()
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
        throw std::runtime_error("Failed to create depth stencil texture");

    hr = m_device->CreateDepthStencilView(m_depthStencil, nullptr, &m_dsv);
    if (FAILED(hr))
        throw std::runtime_error("Failed to create depth stencil view");

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
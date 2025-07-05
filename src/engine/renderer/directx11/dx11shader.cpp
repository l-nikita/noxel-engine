#include "dx11shader.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include "renderer/vertex.hpp"

DX11Shader::DX11Shader(ID3D11Device* device, ID3D11DeviceContext* context, ShaderAsset* asset)
    : m_device(device), m_context(context), m_vs(nullptr), m_ps(nullptr), m_vsBlob(nullptr), m_psBlob(nullptr)
{
    Compile(asset, VERTEX);
    Compile(asset, PIXEL);

    HRESULT hr = m_device->CreateVertexShader(
        m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(),
        nullptr, &m_vs);

    if (FAILED(hr)) 
        throw std::runtime_error("Failed to create vertex shader");

    hr = m_device->CreatePixelShader(
        m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(),
        nullptr, &m_ps);

    if (FAILED(hr)) 
        throw std::runtime_error("Failed to create pixel shader");

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, offsetof(Vertex, Color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,        0, offsetof(Vertex, UV),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = m_device->CreateInputLayout(
        layout, ARRAYSIZE(layout),
        m_vsBlob->GetBufferPointer(),
        m_vsBlob->GetBufferSize(),
        &m_inputLayout);

    if (FAILED(hr))
        throw std::runtime_error("Failed to create input layout");
}

void DX11Shader::Compile(ShaderAsset* asset, ShaderType type)
{
    ID3D10Blob* errorBlob = nullptr;
    HRESULT hr;

    UINT compileFlags = 0;
#ifdef _DEBUG
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    if (type == VERTEX)
    {
        auto vertSrc = asset->GetShaderSource(VERTEX);
        hr = D3DCompile(vertSrc.c_str(), vertSrc.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &m_vsBlob, &errorBlob);
    }
    else if (type == PIXEL)
    {
        auto pixelSrc = asset->GetShaderSource(PIXEL);
        hr = D3DCompile(pixelSrc.c_str(), pixelSrc.length(), nullptr, nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, &m_psBlob, &errorBlob);
    }

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        throw std::runtime_error("Failed to compile shader!");
    }
}

void DX11Shader::Bind()
{
    m_context->IASetInputLayout(m_inputLayout);
    m_context->VSSetShader(m_vs, nullptr, 0);
    m_context->PSSetShader(m_ps, nullptr, 0);
}

void DX11Shader::Unbind()
{
    m_context->IASetInputLayout(nullptr);
    m_context->VSSetShader(nullptr, nullptr, 0);
    m_context->PSSetShader(nullptr, nullptr, 0);
}

void DX11Shader::Destroy()
{
    if (m_vs) m_vs->Release();
    if (m_ps) m_ps->Release();
    if (m_vsBlob) m_vsBlob->Release();
    if (m_psBlob) m_psBlob->Release();
    if (m_inputLayout) m_inputLayout->Release();
}

ID3DBlob* DX11Shader::GetVSBlob() const
{ 
    return m_vsBlob; 
}

ID3DBlob* DX11Shader::GetPSBlob() const
{ 
    return m_psBlob; 
}
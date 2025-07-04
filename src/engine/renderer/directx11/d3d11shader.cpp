#include "d3d11shader.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>

D3D11Shader::D3D11Shader(ID3D11Device* device, ID3D11DeviceContext* context, ShaderAsset* asset) 
    : m_device(device), m_context(context), m_vs(nullptr), m_ps(nullptr), m_vsBlob(nullptr), m_psBlob(nullptr)
{
    Compile(asset, VERTEX);
    Compile(asset, PIXEL);

    HRESULT hr = m_device->CreateVertexShader(
        m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(),
        nullptr, &m_vs);

    if (FAILED(hr)) 
        throw std::runtime_error("Failed to create vertex shader");

    HRESULT hr = m_device->CreatePixelShader(
        m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(),
        nullptr, &m_ps);

    if (FAILED(hr)) 
        throw std::runtime_error("Failed to create pixel shader");
}

void D3D11Shader::Compile(ShaderAsset* asset, ShaderType type)
{
    ID3DBlob* errorBlob;
    HRESULT hr;

    if (type == VERTEX)
    {
        auto vertSrc = asset->GetShaderSource(VERTEX);
        hr = D3DCompile(vertSrc.c_str(), vertSrc.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &m_vsBlob, &errorBlob);
    }
    else if (type == PIXEL)
    {
        auto pixelSrc = asset->GetShaderSource(PIXEL);
        hr = D3DCompile(pixelSrc.c_str(), pixelSrc.length(), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &m_psBlob, &errorBlob);
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

void D3D11Shader::Bind()
{
    m_context->VSSetShader(m_vs, nullptr, 0);
    m_context->PSSetShader(m_ps, nullptr, 0);
}

void D3D11Shader::Unbind()
{
    m_context->VSSetShader(nullptr, nullptr, 0);
    m_context->PSSetShader(nullptr, nullptr, 0);
}

void D3D11Shader::Destroy()
{
    if (m_vs) m_vs->Release();
    if (m_ps) m_ps->Release();
    if (m_vsBlob) m_vsBlob->Release();
    if (m_psBlob) m_psBlob->Release();
}
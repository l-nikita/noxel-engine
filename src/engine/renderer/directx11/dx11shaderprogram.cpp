#include "dx11shaderprogram.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include "renderer/vertex.hpp"
#include "renderer/shaderregistry.hpp"

DX11ShaderProgram::DX11ShaderProgram(ID3D11Device* device, ID3D11DeviceContext* context, ShaderAsset* asset)
    : m_device(device), m_context(context), m_vs(nullptr), m_ps(nullptr), m_vsBlob(nullptr), m_psBlob(nullptr)
{
    if (!ShaderRegistry::IsShaderExist(asset->GetName()))
        FATAL_ERROR("Shader [" + asset->GetName() + "] not found in registry.", CMD::RENDERING);

    Compile(asset, VERTEX);
    Compile(asset, PIXEL);

    HRESULT hr = m_device->CreateVertexShader(
        m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(),
        nullptr, &m_vs);

    if (FAILED(hr)) 
        FATAL_ERROR("DX11: Failed to create vertex dx11 shader", CMD::RENDERING);

    hr = m_device->CreatePixelShader(
        m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(),
        nullptr, &m_ps);

    if (FAILED(hr)) 
        FATAL_ERROR("DX11: Failed to create pixel dx11 shader", CMD::RENDERING);

    m_vShader = ShaderRegistry::Create(asset->GetName() + "VS", this);
    if (!m_vShader)
        FATAL_ERROR("DX11: Failed to create pixel shader", CMD::RENDERING);

    m_pShader = ShaderRegistry::Create(asset->GetName()+"PS", this);
    if(!m_pShader)
        FATAL_ERROR("DX11: Failed to create pixel shader", CMD::RENDERING);
}

void DX11ShaderProgram::Compile(ShaderAsset* asset, ShaderType type)
{
    ID3D10Blob* errorBlob = nullptr;
    HRESULT hr;

    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
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
            CMD_ERROR("Shader compilation error: " + std::string((char*)errorBlob->GetBufferPointer()), CMD::RENDERING);
            errorBlob->Release();
        }

        FATAL_ERROR("Failed to compile shader!", CMD::RENDERING);
    }
}

void DX11ShaderProgram::Bind()
{
    if (m_vShader) m_vShader->Bind();
    if (m_pShader) m_pShader->Bind();
    m_context->VSSetShader(m_vs, nullptr, 0);
    m_context->PSSetShader(m_ps, nullptr, 0);
}

void DX11ShaderProgram::Unbind()
{
    if (m_vShader) m_vShader->Unbind();
    if (m_pShader) m_pShader->Unbind();
    m_context->VSSetShader(nullptr, nullptr, 0);
    m_context->PSSetShader(nullptr, nullptr, 0);
}

void DX11ShaderProgram::Destroy()
{
    if (m_vShader) m_vShader->Destroy();
    if (m_pShader) m_pShader->Destroy();
    if (m_vs) m_vs->Release();
    if (m_ps) m_ps->Release();
    if (m_vsBlob) m_vsBlob->Release();
    if (m_psBlob) m_psBlob->Release();
}

ID3DBlob* DX11ShaderProgram::GetVSBlob() const
{ 
    return m_vsBlob; 
}

ID3DBlob* DX11ShaderProgram::GetPSBlob() const
{ 
    return m_psBlob; 
}
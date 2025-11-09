#pragma once

#include "dx11helper.hpp"
#include "dx11shader.hpp"
#include "dx11cbuffers.hpp"
#include "renderer/directx11/dx11renderer.hpp"
#include "renderer/vertex.hpp"
#include "renderer/rendercamera.hpp"
#include "renderer/shaderregistry.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class SkyboxPassPS : public DX11Shader
{
public:
    SkyboxPassPS(IShaderProgram* program)
        : DX11Shader(program)
    {
        //-----------------------------------------------------------------------------
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = m_device->CreateSamplerState(&sampDesc, &m_samplerState);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create sampler state.", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        D3D11_RASTERIZER_DESC rsDesc = {};
        rsDesc.CullMode = D3D11_CULL_FRONT;
        rsDesc.FillMode = D3D11_FILL_SOLID;

        hr = m_device->CreateRasterizerState(&rsDesc, &m_rasterState);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create rasterizer state.", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthState);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create depth stencil state.", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        m_vars["cubemap"] = g_ErrorTexture;
    }

    ~SkyboxPassPS() { Release(); }

    ShaderType GetType() override { return PIXEL; }

    void Bind()
    {
        m_context->OMSetDepthStencilState(m_depthState, 0);
        m_context->RSSetState(m_rasterState);

        ShaderVar& texVar = GetVar("cubemap");
        if (auto texPtr = std::get_if<ITexture*>(&texVar))
            (*texPtr)->Bind(0);
        else
            g_ErrorTexture->Bind(0);

        m_context->PSSetSamplers(0, 1, &m_samplerState);
    }

    void Unbind()
    {
        m_context->OMSetDepthStencilState(nullptr, 0);
        m_context->RSSetState(nullptr);

        ShaderVar& texVar = GetVar("cubemap");
        if (auto texPtr = std::get_if<ITexture*>(&texVar))
            (*texPtr)->Unbind(0);
        else
            g_ErrorTexture->Unbind(0);

        ID3D11SamplerState* nullSampler[1] = { nullptr };
        m_context->PSSetSamplers(0, 1, nullSampler);
    }

    void Release()
    {
        if (m_samplerState)
        {
            m_samplerState->Release();
            m_samplerState = nullptr;
        }

        if (m_rasterState)
        {
            m_rasterState->Release();
            m_rasterState = nullptr;
        }

        if (m_depthState)
        {
            m_depthState->Release();
            m_depthState = nullptr;
        }
    }

private:
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11RasterizerState* m_rasterState = nullptr;
    ID3D11DepthStencilState* m_depthState = nullptr;
};

REGISTER_SHADER("skyboxpassPS", SkyboxPassPS);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class SkyboxPassVS : public DX11Shader
{
public:
    SkyboxPassVS(IShaderProgram* program)
        : DX11Shader(program)
    {
        //-----------------------------------------------------------------------------
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        HRESULT hr = m_device->CreateInputLayout(
            layout, ARRAYSIZE(layout),
            m_program->GetVSBlob()->GetBufferPointer(),
            m_program->GetVSBlob()->GetBufferSize(),
            &m_inputLayout);

        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create input layout", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(CBTransform);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;
        cbd.StructureByteStride = 0;

        DX11Helper::CreateBuffer(m_device, &cbd, nullptr, &m_cbTransform);
    }

    ~SkyboxPassVS() { Release(); }

    ShaderType GetType() override { return VERTEX; }

    void Bind() 
    { 
        m_context->IASetInputLayout(m_inputLayout);

        auto* camera = g_Renderer->GetCamera();
        CBTransform matricesCB
        {
            glm::transpose(glm::mat4(1.0f)),
            glm::transpose(camera->GetViewMatrix()),
            glm::transpose(camera->GetProjectionMatrix()),
        };

        m_context->UpdateSubresource(m_cbTransform, 0, nullptr, &matricesCB, 0, 0);
        m_context->VSSetConstantBuffers(0, 1, &m_cbTransform);
    }

    void Unbind() 
    { 
        m_context->IASetInputLayout(nullptr);

        ID3D11Buffer* nullBuffer = nullptr;
        m_context->VSSetConstantBuffers(0, 1, &nullBuffer);
    }

    void Release() 
    { 
        if (m_inputLayout)
        {
            m_inputLayout->Release();
            m_inputLayout = nullptr;
        }

        if (m_cbTransform)
        {
            m_cbTransform->Release();
            m_cbTransform = nullptr;
        }
    }

private:
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11Buffer* m_cbTransform = nullptr;
};

REGISTER_SHADER("skyboxpassVS", SkyboxPassVS);
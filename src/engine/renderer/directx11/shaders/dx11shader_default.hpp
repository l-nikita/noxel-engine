#pragma once

#include <stdexcept>
#include <DirectXMath.h>
#include "dx11shader.hpp"
#include "dx11cbuffers.hpp"
#include "../dx11texture.hpp"
#include "../dx11renderer.hpp"
#include "renderer/vertex.hpp"
#include "renderer/irenderer.hpp"
#include "renderer/render_camera.hpp"
#include "renderer/shaderregistry.hpp"

//-----------------------------------------------------------------------------
class DefaultPS : public DX11Shader
{
public:
    DefaultPS(IShaderProgram* program)
        : DX11Shader(program)
    { Init(); }

    ~DefaultPS() { Release(); }

    void Init()
    {
        // parameters, buffers, resources...
        SetParameter("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //D3D11_FILTER_MIN_MAG_MIP_LINEAR
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = m_device->CreateSamplerState(&sampDesc, &m_samplerState);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create sampler state", CMD::RENDERING);

        m_texture = g_ErrorTexture;
    }

    ShaderType GetType() { return PIXEL; }

    void Bind() 
    {
        MaterialValue* texVal = GetParameterValue("basetexture");
        if (auto texPtr = std::get_if<ITexture*>(texVal))
            m_texture = *texPtr;

        if (m_texture) 
            m_texture->Bind();
        else
            g_ErrorTexture->Bind();

        m_context->PSSetSamplers(0, 1, &m_samplerState);
    }

    void Unbind() 
    {
        ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
        m_context->PSSetShaderResources(0, 1, nullSRV);

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

        if (m_texture && m_texture != g_ErrorTexture)
        {
            delete m_texture;
            m_texture = nullptr;
        }
    }

private:
    ID3D11SamplerState* m_samplerState = nullptr;
    ITexture* m_texture = nullptr;
};

REGISTER_SHADER("defaultPS", DefaultPS);

//-----------------------------------------------------------------------------
class DefaultVS : public DX11Shader
{
public:
    DefaultVS(IShaderProgram* program)
        : DX11Shader(program)
    { Init(); }

    ~DefaultVS() { Release(); }

    void Init()
    {
        // parameters, buffers, resources...
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, offsetof(Vertex, Color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,        0, offsetof(Vertex, UV),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        HRESULT hr = m_program->GetDevice()->CreateInputLayout(
            layout, ARRAYSIZE(layout),
            m_program->GetVSBlob()->GetBufferPointer(),
            m_program->GetVSBlob()->GetBufferSize(),
            &m_inputLayout);

        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create input layout", CMD::RENDERING);

        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(CBMatrices);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;
        cbd.StructureByteStride = 0;

        hr = m_device->CreateBuffer(&cbd, nullptr, &m_constantBuffer);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create constant buffer", CMD::RENDERING);
    }

    ShaderType GetType() { return VERTEX; }

    void Bind()
    {
        m_context->IASetInputLayout(m_inputLayout);

        auto* camera = g_Renderer->GetCamera();

        glm::mat4 model = glm::mat4(1.0f);
        MaterialValue* modelVal = GetParameterValue("model");

        if (auto matPtr = std::get_if<glm::mat4>(modelVal))
            model = *matPtr;

        CBMatrices matricesCB
        {
            glm::transpose(model),
            glm::transpose(camera->GetViewMatrix()),
            glm::transpose(camera->GetProjectionMatrix()),
        };
        
        m_context->UpdateSubresource(m_constantBuffer, 0, nullptr, &matricesCB, 0, 0);
        m_context->VSSetConstantBuffers(0, 1, &m_constantBuffer);
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

        if (m_constantBuffer) 
        { 
            m_constantBuffer->Release(); 
            m_constantBuffer = nullptr;
        }
    }

private:
    ID3D11Buffer* m_constantBuffer;
};

REGISTER_SHADER("defaultVS", DefaultVS);
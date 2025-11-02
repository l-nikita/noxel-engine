#pragma once

#include <DirectXMath.h>

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
class GeometryPassPS : public DX11Shader
{
public:
    GeometryPassPS(IShaderProgram* program)
        : DX11Shader(program)
    {
        //D3D11_FILTER_MIN_MAG_MIP_LINEAR
        //D3D11_FILTER_MIN_MAG_MIP_POINT
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
            FATAL_ERROR("DX11: Failed to create sampler state", CMD::RENDERING);

        m_vars["basetexture"] = g_ErrorTexture;
        m_vars["normalmap"] = g_ErrorTexture;
    }

    ~GeometryPassPS() { Release(); }

    ShaderType GetType() override { return PIXEL; }

    void Bind()
    {
        ShaderVar& texVar = GetVar("basetexture");
        if (auto texPtr = std::get_if<ITexture*>(&texVar))
            (*texPtr)->Bind(0);
        else
            g_ErrorTexture->Bind(0);

        ShaderVar& normVar = GetVar("normalmap");
        if (auto normPtr = std::get_if<ITexture*>(&normVar))
            (*normPtr)->Bind(1);
        else
            g_ErrorTexture->Bind(1);

        m_context->PSSetSamplers(0, 1, &m_samplerState);
    }

    void Unbind()
    {
        ShaderVar& texVar = GetVar("basetexture");
        if (auto texPtr = std::get_if<ITexture*>(&texVar))
            (*texPtr)->Unbind(0);
        else
            g_ErrorTexture->Unbind(0);

        ShaderVar& normVar = GetVar("normalmap");
        if (auto normPtr = std::get_if<ITexture*>(&normVar))
            (*normPtr)->Unbind(1);
        else
            g_ErrorTexture->Unbind(1);

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
    }

private:
    ID3D11SamplerState* m_samplerState = nullptr;
};

REGISTER_SHADER("geometrypassPS", GeometryPassPS);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class GeometryPassVS : public DX11Shader
{
public:
    GeometryPassVS(IShaderProgram* program)
        : DX11Shader(program)
    {
        // parameters, buffers, resources...
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, offsetof(Vertex, Normal),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,  0, offsetof(Vertex, Tangent),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BINORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Bitangent),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,      0, offsetof(Vertex, UV),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, offsetof(Vertex, Color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
        cbd.ByteWidth = sizeof(CBTransform);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;
        cbd.StructureByteStride = 0;

        hr = m_device->CreateBuffer(&cbd, nullptr, &m_cbTransform);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create constant buffer", CMD::RENDERING);

        m_vars["model"] = glm::mat4(1.0f);
    }

    ~GeometryPassVS() { Release(); }

    ShaderType GetType() override { return VERTEX; }

    void Bind()
    {
        m_context->IASetInputLayout(m_inputLayout);

        auto* camera = g_Renderer->GetCamera();
        glm::mat4 model = glm::mat4(1.0f);

        ShaderVar& varModel = GetVar("model");
        if (auto matPtr = std::get_if<glm::mat4>(&varModel))
            model = *matPtr;

        CBTransform matricesCB
        {
            glm::transpose(model),
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

REGISTER_SHADER("geometrypassVS", GeometryPassVS);
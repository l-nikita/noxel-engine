#pragma once

#include <DirectXMath.h>

#include "dx11shader.hpp"
#include "dx11cbuffers.hpp"
#include "renderer/directx11/dx11renderer.hpp"
#include "renderer/vertex.hpp"
#include "renderer/render_camera.hpp"
#include "renderer/shaderregistry.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class GizmosPS : public DX11Shader
{
public:
    GizmosPS(IShaderProgram* program)
        : DX11Shader(program)
    { Init(); }

    ~GizmosPS() { Release(); }

    void Init()
    {
        // parameters, buffers, resources...
        SetParameter("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(CBColor);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;
        cbd.StructureByteStride = 0;

        HRESULT hr = m_device->CreateBuffer(&cbd, nullptr, &m_CBColor);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create constant buffer", CMD::RENDERING);
    }

    ShaderType GetType() { return PIXEL; }

    void Bind() 
    {
        glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        MaterialValue* colorVal = GetParameterValue("color");

        if (auto colorPtr = std::get_if<glm::vec4>(colorVal))
            color = *colorPtr;

        CBColor colorCB 
        { 
            color 
        };

        m_context->UpdateSubresource(m_CBColor, 0, nullptr, &colorCB, 0, 0);
        m_context->PSSetConstantBuffers(0, 1, &m_CBColor);
    }

    void Unbind() 
    {
        ID3D11Buffer* nullBuffer = nullptr;
        m_context->PSSetConstantBuffers(0, 1, &nullBuffer);
    }

    void Release()
    {
        if (m_CBColor) 
        { 
            m_CBColor->Release(); 
            m_CBColor = nullptr;
        }
    }

private:
    ID3D11Buffer* m_CBColor;
};

REGISTER_SHADER("gizmosPS", GizmosPS);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class GizmosVS : public DX11Shader
{
public:
    GizmosVS(IShaderProgram* program)
        : DX11Shader(program)
    { Init(); }

    ~GizmosVS() { Release(); }

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

REGISTER_SHADER("gizmosVS", GizmosVS);
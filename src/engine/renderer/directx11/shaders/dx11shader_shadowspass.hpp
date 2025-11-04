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
class ShadowsPassPS : public DX11Shader
{
public:
    ShadowsPassPS(IShaderProgram* program)
        : DX11Shader(program)
    { }

    ~ShadowsPassPS() { Release(); }

    ShaderType GetType() override { return PIXEL; }

    void Bind() { }
    void Unbind() { }
    void Release() { }

private:
};

REGISTER_SHADER("shadowspassPS", ShadowsPassPS);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ShadowsPassVS : public DX11Shader
{
public:
    ShadowsPassVS(IShaderProgram* program)
        : DX11Shader(program)
    {
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

    ~ShadowsPassVS() { Release(); }

    ShaderType GetType() override { return VERTEX; }

    void Bind() 
    { 
        m_context->IASetInputLayout(m_inputLayout);

        auto* camera = g_Renderer->GetCamera();
        auto& camTransform = g_Renderer->GetCamera()->GetTransform();

        // Позиция света
        glm::vec3 lightPos = glm::vec3(0.0f, -3.0f, 6.0f);
        glm::vec3 lightTarget = camTransform.Position;
        glm::vec3 up = glm::vec3(0, 1, 0);

        glm::mat4 lightView = glm::lookAtLH(lightPos, lightTarget, up);

        float shadowMapWidth = 1024.0f;
        float shadowMapHeight = 1024.0f;

        float viewWidth = 30.0f;
        float viewHeight = viewWidth * (shadowMapHeight / shadowMapWidth);

        glm::mat4 lightProj = glm::orthoLH_ZO(-viewWidth / 2, viewWidth / 2, -viewHeight / 2, viewHeight / 2, 1.0f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f);

        ShaderVar& varModel = GetVar("model");
        if (auto matPtr = std::get_if<glm::mat4>(&varModel))
            model = *matPtr;

        CBTransform matricesCB
        {
            glm::transpose(model),
            glm::transpose(lightView),
            glm::transpose(lightProj),
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

REGISTER_SHADER("shadowspassVS", ShadowsPassVS);
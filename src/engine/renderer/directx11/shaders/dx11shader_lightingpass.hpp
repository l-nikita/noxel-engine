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
class LightingPassPS : public DX11Shader
{
public:
    LightingPassPS(IShaderProgram* program)
        : DX11Shader(program)
    {
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

        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.ByteWidth = sizeof(CBLightsData);

        hr = m_device->CreateBuffer(&cbd, nullptr, &m_lightBuffer);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create constant buffer", CMD::RENDERING);
    }

    ~LightingPassPS() { Release(); }

    ShaderType GetType() override { return PIXEL; }

    void Bind()
    {
        static float totalTime = 0.0f;
        totalTime += g_Engine->DeltaTime();

        auto& camTransform = g_Renderer->GetCamera()->GetTransform();

        CBLightsData data{};

        data.Lights[0].Type = LIGHT_POINT;
        data.Lights[0].Pos = glm::vec3(sin(totalTime) * 5.0f, -3.0f, cos(totalTime) * 5.0f);
        data.Lights[0].Color = glm::vec3(1.0f, 0.0f, 0.0f);
        data.Lights[0].Radius = 3.0f;
        data.Lights[0].Cutoff = 0.0f;

        data.Lights[1].Type = LIGHT_SPOT;
        data.Lights[1].Pos = glm::vec3(0.0f, -3.0f, 6.0f);
        data.Lights[1].Dir = glm::normalize(camTransform.Position - data.Lights[1].Pos);
        data.Lights[1].Color = glm::vec3(1.0f, 1.0f, 1.0f);
        data.Lights[1].Radius = 25.0f;
        data.Lights[1].Cutoff = cos(glm::radians(35.0f));

        data.Lights[3].Type = LIGHT_POINT;
        data.Lights[3].Pos = glm::vec3(0.0f, -3.0f, 0.0f);
        data.Lights[3].Color = glm::vec3(0.0f, 0.0f, 1.0f);
        data.Lights[3].Radius = 10.0f;
        data.Lights[3].Cutoff = 0.0f;

        //data.Lights[2].Type = LIGHT_DIRECTIONAL;
        //data.Lights[2].Dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        //data.Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);

        data.ViewPos = camTransform.Position;

        m_context->UpdateSubresource(m_lightBuffer, 0, nullptr, &data, 0, 0);

        m_context->PSSetConstantBuffers(0, 1, &m_lightBuffer);
        m_context->PSSetSamplers(0, 1, &m_samplerState);
    }

    void Unbind()
    {
        ID3D11SamplerState* nullSampler[1] = { nullptr };
        m_context->PSSetSamplers(0, 1, nullSampler);

        m_context->PSSetConstantBuffers(0, 0, nullptr);
    }

    void Release()
    {
        if (m_samplerState)
        {
            m_samplerState->Release();
            m_samplerState = nullptr;
        }

        if (m_lightBuffer)
        {
            m_lightBuffer->Release();
            m_lightBuffer = nullptr;
        }
    }

private:
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11Buffer* m_lightBuffer = nullptr;
};

REGISTER_SHADER("lightingpassPS", LightingPassPS);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class LightingPassVS : public DX11Shader
{
public:
    LightingPassVS(IShaderProgram* program)
        : DX11Shader(program)
    { }

    ~LightingPassVS() { Release(); }

    ShaderType GetType() override { return VERTEX; }

    void Bind() { }
    void Unbind() { }
    void Release() { }
};

REGISTER_SHADER("lightingpassVS", LightingPassVS);
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
        cbd.ByteWidth = sizeof(CBLightData);

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

        CBLightData data{};
        data.lightPos = glm::vec3(
            sin(totalTime) * 1.0f,
            1.0f,
            cos(totalTime) * 1.0f
        );
        //data.lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

        data.lightColor = glm::vec3(3.0f, 3.0f, 3.0f);
        data.cameraPos = g_Renderer->GetCamera()->m_Transform.Position;

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
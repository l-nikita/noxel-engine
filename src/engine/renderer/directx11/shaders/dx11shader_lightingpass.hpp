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
        //-----------------------------------------------------------------------------
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        HRESULT hr = m_device->CreateSamplerState(&sampDesc, &m_samplerState);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create sampler state.", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        D3D11_SAMPLER_DESC samp2Desc = {};
        samp2Desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        samp2Desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samp2Desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samp2Desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samp2Desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        samp2Desc.MinLOD = 0;
        samp2Desc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = m_device->CreateSamplerState(&samp2Desc, &m_sampler2State);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create sampler state.", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.ByteWidth = sizeof(CBLightsData);

        hr = m_device->CreateBuffer(&cbd, nullptr, &m_lightBuffer);
        if (FAILED(hr))
            FATAL_ERROR("DX11: Failed to create constant buffer", CMD::RENDERING);

        m_vars["shadowsmap"] = g_ErrorTexture;


        m_light1 = new Light(LIGHT_POINT);
        m_light1->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
        m_light1->SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
        m_light1->SetRadius(3.0f);

        m_light2 = new Light(LIGHT_SPOT);
        m_light2->SetColor(glm::vec3(2.0f, 2.0f, 2.0f));
        m_light2->SetPosition(glm::vec3(0.0f, -3.0f, 7.0f));
        m_light2->SetRadius(25.0f);
        m_light2->SetCutoff(cos(glm::radians(15.0f)));

        Light* light3 = new Light(LIGHT_POINT);
        light3->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
        light3->SetPosition(glm::vec3(0.0f, -3.0f, 0.0f));
        light3->SetRadius(10.0f);

        LightManager::GetInstance().AddLight(m_light1);
        LightManager::GetInstance().AddLight(m_light2);
        LightManager::GetInstance().AddLight(light3);
    }

    ~LightingPassPS() { Release(); }

    ShaderType GetType() override { return PIXEL; }

    void Bind()
    {
        static float totalTime = 0.0f;
        totalTime += g_Engine->DeltaTime();

        auto& camTransform = g_Renderer->GetCamera()->GetTransform();

        CBLightsData data{};
        data.ViewPos = camTransform.Position;
        
        auto lights = LightManager::GetInstance().GetLights();
        for (int i = 0; i < lights.size(); i++)
        {
            if (i >= MAX_LIGHTS) continue;

            auto light = lights[i];
            auto& transform = light->GetTransform();

            auto type = light->GetType();
            auto& dataLight = data.Lights[i];

            dataLight.Type = type;
            dataLight.Color = light->GetColor();
            dataLight.Radius = light->GetRadius();
            dataLight.Cutoff = light->GetCutoff();
            dataLight.Pos = transform.Position;
            if (type == LIGHT_SPOT)
                dataLight.Dir = transform.Forward();
        }

        auto pos = glm::vec3(sin(totalTime) * 5.0f, -3.0f, cos(totalTime) * 5.0f);
        m_light1->SetPosition(pos);

        glm::vec3 dir = glm::normalize(m_light2->GetPosition() - pos);
        m_light2->SetRotation(glm::quatLookAt(dir, m_light2->GetTransform().Up()));

        m_context->UpdateSubresource(m_lightBuffer, 0, nullptr, &data, 0, 0);
        m_context->PSSetConstantBuffers(0, 1, &m_lightBuffer);

        m_context->PSSetSamplers(0, 1, &m_samplerState);
        m_context->PSSetSamplers(1, 1, &m_sampler2State);

        //ShaderVar& varModel = GetVar("shadowsmap");
        //if (auto texPtr = std::get_if<ITexture*>(&varModel))
        //    (*texPtr)->Bind(3);
    }

    void Unbind()
    {
        m_context->PSSetConstantBuffers(0, 0, nullptr);

        ID3D11SamplerState* nullSampler[1] = { nullptr };
        m_context->PSSetSamplers(0, 1, nullSampler);
        m_context->PSSetSamplers(1, 1, nullSampler);

        //ShaderVar& varModel = GetVar("shadowsmap");
        //if (auto texPtr = std::get_if<ITexture*>(&varModel))
        //    (*texPtr)->Unbind(3);
    }

    void Release()
    {
        if (m_samplerState)
        {
            m_samplerState->Release();
            m_samplerState = nullptr;
        }

        if (m_sampler2State)
        {
            m_sampler2State->Release();
            m_sampler2State = nullptr;
        }

        if (m_lightBuffer)
        {
            m_lightBuffer->Release();
            m_lightBuffer = nullptr;
        }
    }

private:
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11SamplerState* m_sampler2State = nullptr;
    ID3D11Buffer* m_lightBuffer = nullptr;

    Light* m_light1 = nullptr;
    Light* m_light2 = nullptr;
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
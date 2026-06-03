#pragma once

#include "dx11cbuffers.hpp"
#include "dx11shader.hpp"
#include "dbg/debug.hpp"
#include "renderer/rendercamera.hpp"
#include "renderer/shaderregistry.hpp"
#include "renderer/vertex.hpp"
#include "renderer/directx11/dx11helper.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class GeometryPassPS : public DX11Shader
{
public:
    GeometryPassPS( IShaderProgram* program )
        : DX11Shader(program)
    {
        //-----------------------------------------------------------------------------
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
        if ( FAILED(hr) )
            FATAL_ERROR("DX11: Failed to create sampler state", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.ByteWidth = sizeof(CBViewData);

        DX11Helper::CreateBuffer(m_device, &cbd, nullptr, &m_viewBuffer);

        //-----------------------------------------------------------------------------
        D3D11_BUFFER_DESC cbd2 = {};
        cbd2.Usage = D3D11_USAGE_DEFAULT;
        cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd2.ByteWidth = sizeof(CBTint);

        DX11Helper::CreateBuffer(m_device, &cbd2, nullptr, &m_tintBuffer);

        //-----------------------------------------------------------------------------
        m_vars[ "tint" ] = glm::vec4(1, 1, 1, 1);
        m_vars[ "emission" ] = 0.0f;
        m_vars[ "color" ] = g_BlackTexture;
        m_vars[ "normal" ] = g_BlackTexture;
        m_vars[ "rough" ] = g_BlackTexture;
        m_vars[ "metal" ] = g_BlackTexture;
        m_vars[ "ao" ] = g_BlackTexture;
        m_vars[ "height" ] = g_BlackTexture;
    }

    ~GeometryPassPS()
    {
        Release();
    }

    ShaderType GetType() override
    {
        return PIXEL;
    }

    void Bind()
    {
        ShaderVar& colorVar = GetVar("color");
        if ( auto colorPtr = std::get_if<ITexture*>(&colorVar) )
            ( *colorPtr )->Bind(0);

        ShaderVar& normVar = GetVar("normal");
        if ( auto normPtr = std::get_if<ITexture*>(&normVar) )
            ( *normPtr )->Bind(1);

        ShaderVar& roughVar = GetVar("rough");
        if ( auto roughPtr = std::get_if<ITexture*>(&roughVar) )
            ( *roughPtr )->Bind(2);

        ShaderVar& metalVar = GetVar("metal");
        if ( auto metalPtr = std::get_if<ITexture*>(&metalVar) )
            ( *metalPtr )->Bind(3);

        ShaderVar& aoVar = GetVar("ao");
        if ( auto aoPtr = std::get_if<ITexture*>(&aoVar) )
            ( *aoPtr )->Bind(4);

        ShaderVar& heightVar = GetVar("height");
        if ( auto heightPtr = std::get_if<ITexture*>(&heightVar) )
            ( *heightPtr )->Bind(5);

        CBViewData data
        {
            glm::vec4(g_Renderer->GetCamera()->GetPosition(), 1),
        };

        m_context->UpdateSubresource(m_viewBuffer, 0, nullptr, &data, 0, 0);
        m_context->PSSetConstantBuffers(0, 1, &m_viewBuffer);

        glm::vec4 tint = glm::vec4(1);
        float emis = 0.0f;

        ShaderVar& tintVar = GetVar("tint");
        if ( auto tintPtr = std::get_if<glm::vec4>(&tintVar) )
            tint = *tintPtr;

        ShaderVar& emissionVar = GetVar("emission");
        if ( auto emissionPtr = std::get_if<float>(&emissionVar) )
            emis = *emissionPtr;

        CBTint data2 = {};
        data2.Tint = tint;
        data2.Emission = emis;

        m_context->UpdateSubresource(m_tintBuffer, 0, nullptr, &data2, 0, 0);
        m_context->PSSetConstantBuffers(1, 1, &m_tintBuffer);

        m_context->PSSetSamplers(0, 1, &m_samplerState);
    }

    void Unbind()
    {
        ShaderVar& colorVar = GetVar("color");
        if ( auto colorPtr = std::get_if<ITexture*>(&colorVar) )
            ( *colorPtr )->Unbind(0);

        ShaderVar& normVar = GetVar("normal");
        if ( auto normPtr = std::get_if<ITexture*>(&normVar) )
            ( *normPtr )->Unbind(1);

        ShaderVar& roughVar = GetVar("rough");
        if ( auto roughPtr = std::get_if<ITexture*>(&roughVar) )
            ( *roughPtr )->Unbind(2);

        ShaderVar& metalVar = GetVar("metal");
        if ( auto metalPtr = std::get_if<ITexture*>(&metalVar) )
            ( *metalPtr )->Unbind(3);

        ShaderVar& aoVar = GetVar("ao");
        if ( auto aoPtr = std::get_if<ITexture*>(&aoVar) )
            ( *aoPtr )->Unbind(4);

        ShaderVar& heightVar = GetVar("height");
        if ( auto heightPtr = std::get_if<ITexture*>(&heightVar) )
            ( *heightPtr )->Unbind(5);

        m_context->PSSetConstantBuffers(0, 0, nullptr);

        ID3D11SamplerState* nullSampler[ 2 ] = {nullptr};
        m_context->PSSetSamplers(0, 2, nullSampler);
    }

    void Release()
    {
        if ( m_samplerState )
        {
            m_samplerState->Release();
            m_samplerState = nullptr;
        }

        if ( m_viewBuffer )
        {
            m_viewBuffer->Release();
            m_viewBuffer = nullptr;
        }

        if ( m_tintBuffer )
        {
            m_tintBuffer->Release();
            m_tintBuffer = nullptr;
        }
    }

private:
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11Buffer* m_viewBuffer = nullptr;
    ID3D11Buffer* m_tintBuffer = nullptr;
};

REGISTER_SHADER(




    "geometrypassPS"
    ,
    GeometryPassPS
);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class GeometryPassVS : public DX11Shader
{
public:
    GeometryPassVS( IShaderProgram* program )
        : DX11Shader(program)
    {
        //-----------------------------------------------------------------------------
        D3D11_INPUT_ELEMENT_DESC layout[ ] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Tangent), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {
                "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Bitangent), D3D11_INPUT_PER_VERTEX_DATA,
                0
            },
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, UV), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, Color), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        HRESULT hr = m_device->CreateInputLayout(
            layout, ARRAYSIZE(layout),
            m_program->GetVSBlob()->GetBufferPointer(),
            m_program->GetVSBlob()->GetBufferSize(),
            &m_inputLayout);

        if ( FAILED(hr) )
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

        //-----------------------------------------------------------------------------
        m_vars[ "model" ] = glm::mat4(1.0f);
    }

    ~GeometryPassVS()
    {
        Release();
    }

    ShaderType GetType() override
    {
        return VERTEX;
    }

    void Bind()
    {
        m_context->IASetInputLayout(m_inputLayout);

        auto* camera = g_Renderer->GetCamera();
        glm::mat4 model = glm::mat4(1.0f);

        ShaderVar& varModel = GetVar("model");
        if ( auto matPtr = std::get_if<glm::mat4>(&varModel) )
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
        if ( m_inputLayout )
        {
            m_inputLayout->Release();
            m_inputLayout = nullptr;
        }

        if ( m_cbTransform )
        {
            m_cbTransform->Release();
            m_cbTransform = nullptr;
        }
    }

private:
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11Buffer* m_cbTransform = nullptr;
};

REGISTER_SHADER(




    "geometrypassVS"
    ,
    GeometryPassVS
);

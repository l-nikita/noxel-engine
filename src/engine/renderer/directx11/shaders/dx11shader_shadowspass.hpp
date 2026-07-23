#pragma once

#include "dx11cbuffers.hpp"
#include "dx11shader.hpp"
#include "dbg/debug.hpp"
#include "renderer/rendercamera.hpp"
#include "renderer/shaderregistry.hpp"
#include "renderer/vertex.hpp"
#include "renderer/directx11/dx11helper.hpp"
#include "renderer/directx11/dx11renderer.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ShadowsPassPS : public DX11Shader
{
public:
    ShadowsPassPS( IShaderProgram* program )
        : DX11Shader(program)
    {
    }

    ~ShadowsPassPS()
    {
        Release();
    }

    ShaderType GetType() override
    {
        return PIXEL;
    }

    void Bind()
    {
    }

    void Unbind()
    {
    }

    void Release()
    {
    }

private:
};

REGISTER_SHADER(




    "shadowspassPS"
    ,
    ShadowsPassPS
);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ShadowsPassVS : public DX11Shader
{
public:
    ShadowsPassVS( IShaderProgram* program )
        : DX11Shader(program)
    {
        //-----------------------------------------------------------------------------
        D3D11_INPUT_ELEMENT_DESC layout[ ] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        HRESULT hr = m_device->CreateInputLayout(
            layout, ARRAYSIZE(layout),
            m_program->GetVSBlob()->GetBufferPointer(),
            m_program->GetVSBlob()->GetBufferSize(),
            &m_inputLayout);

        if ( FAILED(hr) )
            FATAL_ERROR("DX11: Failed to create input layout.", CMD::RENDERING);

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
        // Offset shadow casters in depth space. Receiver-side bias alone cannot
        // reliably remove acne from surfaces viewed at a grazing angle.
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FrontCounterClockwise = FALSE;
        rasterDesc.DepthBias = 1000;
        rasterDesc.DepthBiasClamp = 0.01f;
        rasterDesc.SlopeScaledDepthBias = 2.0f;
        rasterDesc.DepthClipEnable = TRUE;

        hr = m_device->CreateRasterizerState(&rasterDesc, &m_shadowRasterState);
        if ( FAILED(hr) )
            FATAL_ERROR("DX11: Failed to create shadow rasterizer state.", CMD::RENDERING);

        //-----------------------------------------------------------------------------
        m_vars[ "model" ] = glm::mat4(1.0f);
        m_vars[ "lightview" ] = glm::mat4(1.0f);
        m_vars[ "lightproj" ] = glm::mat4(1.0f);
    }

    ~ShadowsPassVS()
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
        m_context->RSSetState(m_shadowRasterState);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 lightView = glm::mat4(1.0f);
        glm::mat4 lightProj = glm::mat4(1.0f);

        ShaderVar& varLightView = GetVar("lightview");
        if ( auto matPtr = std::get_if<glm::mat4>(&varLightView) )
            lightView = *matPtr;

        ShaderVar& varLightProj = GetVar("lightproj");
        if ( auto matPtr = std::get_if<glm::mat4>(&varLightProj) )
            lightProj = *matPtr;

        ShaderVar& varModel = GetVar("model");
        if ( auto matPtr = std::get_if<glm::mat4>(&varModel) )
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
        m_context->RSSetState(nullptr);

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

        if ( m_shadowRasterState )
        {
            m_shadowRasterState->Release();
            m_shadowRasterState = nullptr;
        }
    }

private:
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11Buffer* m_cbTransform = nullptr;
    ID3D11RasterizerState* m_shadowRasterState = nullptr;
};

REGISTER_SHADER(




    "shadowspassVS"
    ,
    ShadowsPassVS
);

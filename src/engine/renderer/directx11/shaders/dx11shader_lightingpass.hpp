#pragma once

#include <array>

#include "dx11cbuffers.hpp"
#include "dx11shader.hpp"
#include "dbg/debug.hpp"
#include "renderer/lighting.hpp"
#include "renderer/render_debug.hpp"
#include "renderer/rendercamera.hpp"
#include "renderer/shaderregistry.hpp"
#include "renderer/directx11/dx11helper.hpp"
#include "renderer/directx11/dx11renderer.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class LightingPassPS : public DX11Shader
{
public:
    LightingPassPS( IShaderProgram* program )
        : DX11Shader( program )
    {
        //-----------------------------------------------------------------------------
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        HRESULT hr = m_device->CreateSamplerState( &sampDesc, &m_samplerState );
        if ( FAILED( hr ) )
            FATAL_ERROR( "DX11: Failed to create sampler state.", CMD::RENDERING );

        //-----------------------------------------------------------------------------
        D3D11_SAMPLER_DESC samp2Desc = {};
        samp2Desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        // D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT // D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT
        samp2Desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        samp2Desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        samp2Desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        samp2Desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        samp2Desc.MinLOD = 0;
        samp2Desc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = m_device->CreateSamplerState( &samp2Desc, &m_sampler2State );
        if ( FAILED( hr ) )
            FATAL_ERROR( "DX11: Failed to create sampler state.", CMD::RENDERING );

        //-----------------------------------------------------------------------------
        D3D11_SAMPLER_DESC shadowDepthSamplerDesc = {};
        shadowDepthSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        shadowDepthSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        shadowDepthSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        shadowDepthSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        shadowDepthSamplerDesc.BorderColor[ 0 ] = 1.0f;
        shadowDepthSamplerDesc.BorderColor[ 1 ] = 1.0f;
        shadowDepthSamplerDesc.BorderColor[ 2 ] = 1.0f;
        shadowDepthSamplerDesc.BorderColor[ 3 ] = 1.0f;
        shadowDepthSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        shadowDepthSamplerDesc.MinLOD = 0;
        shadowDepthSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = m_device->CreateSamplerState( &shadowDepthSamplerDesc, &m_shadowDepthSamplerState );
        if ( FAILED( hr ) )
            FATAL_ERROR( "DX11: Failed to create shadow depth sampler state.", CMD::RENDERING );

        //-----------------------------------------------------------------------------
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.ByteWidth = sizeof( CBLightsData );

        DX11Helper::CreateBuffer( m_device, &cbd, nullptr, &m_lightBuffer );

        //-----------------------------------------------------------------------------
        cbd.ByteWidth = sizeof( CBDebugData );
        DX11Helper::CreateBuffer( m_device, &cbd, nullptr, &m_debugBuffer );

        //-----------------------------------------------------------------------------
        m_light1 = new Light( LIGHT_POINT );
        m_light1->SetColor( glm::vec3( 1.0f, 0.0f, 0.0f ) );
        m_light1->SetPosition( glm::vec3( 1.0f, 0.0f, 0.0f ) );
        m_light1->SetRadius( 3.0f );

        m_light2 = new Light( LIGHT_SPOT, true );
        m_light2->SetColor( glm::vec3( 1.0f, 1.0f, 1.0f ) );
        m_light2->SetPosition( glm::vec3( 0.0f, -3.0f, 7.0f ) );
        m_light2->SetRadius( 45.0f );
        m_light2->SetFOV( 70.0f );
        glm::vec3 forward = m_light2->GetTransform().Forward();
        m_light2->SetRotation( glm::rotation( forward, -forward ) );

        Light* light3 = new Light( LIGHT_POINT );
        light3->SetColor( glm::vec3( 0.0f, 0.0f, 1.0f ) );
        light3->SetPosition( glm::vec3( 0.0f, -1.0f, 0.0f ) );
        light3->SetRadius( 5.0f );

        m_light4 = new Light( LIGHT_SPOT, true );
        m_light4->SetColor( glm::vec3( 1.0f, 0.0f, 1.0f ) );
        m_light4->SetPosition( glm::vec3( -9.0f, -3.0f, -10.0f ) );
        m_light4->SetRotation( glm::angleAxis( glm::radians( 45.0f ), m_light4->Up() ) );
        m_light4->SetRadius( 20.0f );
        m_light4->SetFOV( 90.0f );

        m_directionalLight = new Light( LIGHT_DIRECTIONAL, true, 4096 );
        m_directionalLight->SetColor( glm::vec3( 0.25f, 0.23f, 0.19f ) );
        const glm::vec3 sunDirection = glm::normalize( glm::vec3( -0.35f, -1.0f, 0.25f ) );
        m_directionalLight->SetRotation(
            glm::rotation( glm::vec3( 0.0f, 0.0f, 1.0f ), sunDirection )
        );
        m_directionalLight->SetPosition(
            glm::vec3( 0.0f, -3.0f, 0.0f ) - sunDirection * 35.0f
        );
        m_directionalLight->SetRadius( 60.0f );

        LightManager::GetInstance().AddLight( m_light1 );
        LightManager::GetInstance().AddLight( m_light2 );
        LightManager::GetInstance().AddLight( light3 );
        LightManager::GetInstance().AddLight( m_light4 );
        LightManager::GetInstance().AddLight( m_directionalLight );

        //-----------------------------------------------------------------------------
        m_vars[ "cubemap" ] = g_BlackTexture;
    }

    ~LightingPassPS()
    {
        Release();
    }

    ShaderType GetType() override
    {
        return PIXEL;
    }

    void Bind()
    {
        static float totalTime = 0.0f;
        totalTime += g_Engine->DeltaTime();

        auto camera = g_Renderer->GetCamera();
        auto& camTransform = camera->GetTransform();

        CBLightsData data{};
        data.ViewPos = camTransform.Position;

        CBDebugData debugData{};
        debugData.DebugView = g_RenderDebugSettings.LightingView;
        debugData.DebugLightIndex = g_RenderDebugSettings.SelectedLight;
        debugData.DebugOverlayOpacity = g_RenderDebugSettings.OverlayOpacity;
        debugData.DebugExposure = g_RenderDebugSettings.Exposure;
        debugData.CameraForward = camTransform.Forward();
        debugData.CameraNear = camera->GetNear();
        debugData.CameraFar = camera->GetFar();
        debugData.IBLEnabled = g_RenderDebugSettings.IBLEnabled ? 1 : 0;

        ID3D11ShaderResourceView* shadowMapSRVs[ MAX_SHADOWS_MAPS ] = {nullptr};
        int shadowCount = 0;

        auto& lights = LightManager::GetInstance().GetLights();
        for ( int i = 0; i < lights.size(); i++ )
        {
            if ( i >= MAX_LIGHTS ) continue;

            auto light = lights[ i ];
            if ( !light )
                continue;

            auto& transform = light->GetTransform();

            auto type = light->GetType();
            auto& dataLight = data.Lights[ i ];

            dataLight.Type = type;
            dataLight.Color = light->GetColor();
            dataLight.Radius = light->GetRadius();
            dataLight.Pos = transform.Position;
            dataLight.ShadowMapIndex = -1;
            dataLight.ShadowSoftness = light->GetShadowSoftness();

            if ( dataLight.Type == LIGHT_SPOT || dataLight.Type == LIGHT_DIRECTIONAL )
            {
                dataLight.Dir = transform.Forward();
                dataLight.View = glm::transpose( light->GetViewMatrix() );
                dataLight.Projection = glm::transpose( light->GetProjectionMatrix() );

                if ( light->IsShadowsEnabled() && shadowCount < MAX_SHADOWS_MAPS )
                {
                    dataLight.ShadowMapIndex = shadowCount;
                    auto srv = static_cast<DX11Texture*>(
                        light->GetShadowMap()->GetDepth()->GetTexture()
                    )->GetSRV();
                    shadowMapSRVs[ shadowCount ] = srv;
                    shadowCount++;
                }
            }

            if ( dataLight.Type == LIGHT_SPOT )
            {
                dataLight.Cutoff = cos( glm::radians( light->GetFOV() * 0.5f ) );
            }
        }

        if ( shadowCount > 0 )
            m_context->PSSetShaderResources( 4, shadowCount, shadowMapSRVs );

        ShaderVar& cubemapVar = GetVar( "cubemap" );
        if ( auto cubemapPtr = std::get_if<ITexture*>( &cubemapVar ) )
            ( *cubemapPtr )->Bind( 10 );

        //------------------------
        auto pos = glm::vec3( sin( totalTime ) * 5.0f, -3.0f, cos( totalTime ) * 5.0f );
        m_light1->SetPosition( pos );

        //glm::vec3 dir = glm::normalize(m_light2->GetPosition() - pos);
        glm::vec3 dir = glm::normalize( m_light2->GetPosition() - camTransform.Position );
        m_light2->SetRotation( glm::quatLookAt( dir, m_light2->GetTransform().Up() ) );
        //m_light2->SetPosition(camTransform.Position + camTransform.Right() * 2.0f);
        //m_light2->SetRotation(camTransform.Rotation);
        //------------------------

        m_context->UpdateSubresource( m_lightBuffer, 0, nullptr, &data, 0, 0 );
        m_context->PSSetConstantBuffers( 0, 1, &m_lightBuffer );

        m_context->UpdateSubresource( m_debugBuffer, 0, nullptr, &debugData, 0, 0 );
        m_context->PSSetConstantBuffers( 1, 1, &m_debugBuffer );

        m_context->PSSetSamplers( 0, 1, &m_samplerState );
        m_context->PSSetSamplers( 1, 1, &m_sampler2State );
        m_context->PSSetSamplers( 2, 1, &m_shadowDepthSamplerState );
    }

    void Unbind()
    {
        ID3D11ShaderResourceView* nullSRVs[ MAX_SHADOWS_MAPS ] = {nullptr};
        m_context->PSSetShaderResources( 4, MAX_SHADOWS_MAPS, nullSRVs );

        ShaderVar& cubemapVar = GetVar( "cubemap" );
        if ( auto cubemapPtr = std::get_if<ITexture*>( &cubemapVar ) )
            ( *cubemapPtr )->Unbind( 10 );

        ID3D11Buffer* nullBuffers[ 2 ] = {nullptr};
        m_context->PSSetConstantBuffers( 0, 2, nullBuffers );

        ID3D11SamplerState* nullSampler[ 3 ] = {nullptr};
        m_context->PSSetSamplers( 0, 3, nullSampler );
    }

    void Release()
    {
        if ( m_samplerState )
        {
            m_samplerState->Release();
            m_samplerState = nullptr;
        }

        if ( m_sampler2State )
        {
            m_sampler2State->Release();
            m_sampler2State = nullptr;
        }

        if ( m_shadowDepthSamplerState )
        {
            m_shadowDepthSamplerState->Release();
            m_shadowDepthSamplerState = nullptr;
        }

        if ( m_lightBuffer )
        {
            m_lightBuffer->Release();
            m_lightBuffer = nullptr;
        }

        if ( m_debugBuffer )
        {
            m_debugBuffer->Release();
            m_debugBuffer = nullptr;
        }
    }

private:
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11SamplerState* m_sampler2State = nullptr;
    ID3D11SamplerState* m_shadowDepthSamplerState = nullptr;
    ID3D11Buffer* m_lightBuffer = nullptr;
    ID3D11Buffer* m_debugBuffer = nullptr;

    Light* m_light1 = nullptr;
    Light* m_light2 = nullptr;
    Light* m_light4 = nullptr;
    Light* m_directionalLight = nullptr;
};

REGISTER_SHADER(




    "lightingpassPS"
    ,
    LightingPassPS
);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class LightingPassVS : public DX11Shader
{
public:
    LightingPassVS( IShaderProgram* program )
        : DX11Shader( program )
    {
    }

    ~LightingPassVS()
    {
        Release();
    }

    ShaderType GetType() override
    {
        return VERTEX;
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
};

REGISTER_SHADER( "lightingpassVS", LightingPassVS );

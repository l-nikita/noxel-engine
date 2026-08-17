#include <Graphics/GraphicsTools/interface/MapHelper.hpp>

#include "renderer/deferred/passes/lightingpass.hpp"
#include "iengine.hpp"
#include "player.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "dbg/debug.hpp"
#include "input/iinput.hpp"
#include "renderer/factory.hpp"
#include "renderer/shader_cbuffers.hpp"
#include "renderer/deferred/deferred_render_pipeline.hpp"

namespace render::deferred::passes
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    LightingPass::LightingPass( const DeferredPassContext& ctx )
        : DeferredPass( ctx )
    {
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void LightingPass::Setup()
    {
        SetupPSO();
        SetupConstantBuffers();
        SetupSRB();

        auto& lights = LightManager::Get();

        m_light1 = lights.CreateLight( LIGHT_POINT );
        m_light1->SetColor( glm::vec3( 1.0f, 0.0f, 0.0f ) )
                .SetPosition( glm::vec3( 1.0f, 0.0f, 0.0f ) )
                .SetRadius( 3.0f );

        m_light2 = lights.CreateLight( LIGHT_SPOT, true );
        glm::vec3 forward = m_light2->GetTransform().Forward();

        m_light2->SetColor( glm::vec3( 1.0f, 1.0f, 1.0f ) )
                .SetPosition( glm::vec3( 0.0f, -3.0f, 7.0f ) )
                .SetRadius( 45.0f )
                .SetOuterCutoff( 70.0f )
                .SetRotation( glm::rotation( forward, -forward ) );

        auto light3 = lights.CreateLight( LIGHT_POINT );
        light3->SetColor( glm::vec3( 0.0f, 0.0f, 1.0f ) )
                .SetPosition( glm::vec3( 0.0f, -1.0f, 0.0f ) )
                .SetRadius( 5.0f );

        m_light4 = lights.CreateLight( LIGHT_SPOT, true );
        m_light4->SetColor( glm::vec3( 1.0f, 0.0f, 1.0f ) )
                .SetPosition( glm::vec3( -9.0f, -3.0f, -10.0f ) )
                .SetRotation( glm::angleAxis( glm::radians( 45.0f ), m_light4->Up() ) )
                .SetRadius( 20.0f )
                .SetOuterCutoff( 90.0f );

        /*
        const glm::vec3 sunDirection = glm::normalize( glm::vec3( -0.35f, -1.0f, 0.25f ) );

        m_sunLight = lights.CreateLight( LIGHT_DIRECTIONAL, true, 4096 );
        m_sunLight->SetColor( glm::vec3( 0.25f, 0.23f, 0.19f ) )
                .SetRotation( glm::rotation( glm::vec3( 0.0f, 0.0f, 1.0f ), sunDirection ) )
                .SetPosition( glm::vec3( 0.0f, -3.0f, 0.0f ) - sunDirection * 35.0f )
                .SetRadius( 60.0f );
        */

        m_flashlight = lights.CreateLight( LIGHT_SPOT, true );
        m_flashlight->SetColor( glm::vec3( 1.0f, 1.0f, 1.0f ) )
                .SetBrightness( 1.5f )
                .SetRadius( 100.0f )
                .SetOuterCutoff( 75.0f )
                .SetInnerCutoff( 60.0f );

        if ( auto cubeVar = m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tCubemap" ) )
        {
            auto factory = m_render->GetFactory();

            auto skyboxMat = g_Assets->FindAsset( "materials/skybox.nmt", assets::Type::MATERIAL );
            if ( !skyboxMat )
                FATAL_ERROR( "Skybox material not found", cmd::RENDERING );

            m_skyboxTex = factory->CreateTextureCube( skyboxMat.value() );
            cubeVar->Set( m_skyboxTex->GetDefaultView( TEXTURE_VIEW_SHADER_RESOURCE ) );
        }
    }

    //-----------------------------------------------------------------------------
    void LightingPass::Render( const LightingPassContext& ctx )
    {
        auto context = m_render->GetContext();
        auto swapChain = m_render->GetSwapChain();

        auto gbuffer = ctx.GBuffer;
        auto camera = ctx.Camera;

        ITextureView* backBuffer =
                swapChain->GetCurrentBackBufferRTV();

        context->SetRenderTargets(
            1,
            &backBuffer,
            nullptr,
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );

        context->SetPipelineState( m_PSO );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tColor" )
                ->Set( gbuffer->GetColor().SRV );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tNormal" )
                ->Set( gbuffer->GetNormal().SRV );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tPosition" )
                ->Set( gbuffer->GetPosition().SRV );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tSpecular" )
                ->Set( gbuffer->GetSpecular().SRV );

        context->CommitShaderResources(
            m_SRB,
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );

        //------------------------
        static float totalTime = 0.0f;
        totalTime += g_Engine->DeltaTime();

        auto pos = glm::vec3( sin( totalTime ) * 5.0f, -3.0f, cos( totalTime ) * 5.0f );
        m_light1->SetPosition( pos );

        m_light2->LookAt( camera->GetPosition() );

        m_flashlight->SetPosition( camera->GetPosition() );
        m_flashlight->SetRotation( camera->GetRotation() );

        if ( g_Player )
            g_Player->m_Flashlight = m_flashlight;

        //m_light2->SetPosition(camTransform.Position + camTransform.Right() * 2.0f);
        //m_light2->SetRotation(camTransform.Rotation);
        //------------------------

        UpdateLights();
        UpdateView( camera );

        DrawAttribs draw;
        draw.NumVertices = 3;

#ifdef _DEBUG
        draw.Flags = DRAW_FLAG_VERIFY_ALL;
#endif

        context->Draw( draw );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void LightingPass::SetupPSO()
    {
        auto swapChain = m_render->GetSwapChain();
        auto factory = m_render->GetFactory();

        GraphicsPipelineStateCreateInfo info;
        info.PSODesc.Name = "Lighting Pass";
        info.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        //-----------------------------------------------------------------------------
        auto& gp = info.GraphicsPipeline;

        gp.NumRenderTargets = 1;
        gp.RTVFormats[ 0 ] = swapChain->GetDesc().ColorBufferFormat;
        gp.DSVFormat = TEX_FORMAT_UNKNOWN;
        gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        gp.DepthStencilDesc.DepthEnable = False;
        gp.DepthStencilDesc.DepthWriteEnable = False;
        gp.RasterizerDesc.CullMode = CULL_MODE_NONE;

        //-----------------------------------------------------------------------------
        auto lightingPass = g_Assets->FindAsset( "shaders/lightingpass.nshader", assets::Type::SHADER );
        if ( !lightingPass )
            FATAL_ERROR( "LightingPass shader not found", cmd::RENDERING );

        m_shaderData = g_Assets->LoadData<assets::ShaderData>( lightingPass.value() );
        assert( m_shaderData );

        auto [ vs, ps ] = factory->CreateShader( lightingPass.value() );
        info.pVS = vs;
        info.pPS = ps;

        factory->CreatePipelineState( info, *m_shaderData, &m_PSO );
    }

    //-----------------------------------------------------------------------------
    void LightingPass::SetupConstantBuffers()
    {
        auto device = m_render->GetDevice();

        BufferDesc cbTransDesc;
        cbTransDesc.Name = "LightingPass LightsData Constant Buffer";
        cbTransDesc.Size = sizeof( CBLightsData );
        cbTransDesc.Usage = USAGE_DYNAMIC;
        cbTransDesc.BindFlags = BIND_UNIFORM_BUFFER;
        cbTransDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer( cbTransDesc, nullptr, &m_lightingCB );

        BufferDesc cbViewDesc;
        cbViewDesc.Name = "LightingPass ViewData Constant Buffer";
        cbViewDesc.Size = sizeof( CBViewData );
        cbViewDesc.Usage = USAGE_DYNAMIC;
        cbViewDesc.BindFlags = BIND_UNIFORM_BUFFER;
        cbViewDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer( cbViewDesc, nullptr, &m_viewCB );

        auto lightingVar =
                m_PSO->GetStaticVariableByName( SHADER_TYPE_PIXEL, "LightsData" );

        auto viewVar =
                m_PSO->GetStaticVariableByName( SHADER_TYPE_PIXEL, "ViewData" );

        if ( !lightingVar || !viewVar )
            FATAL_ERROR( "Lighting shader constant buffer was not found", cmd::RENDERING );

        lightingVar->Set( m_lightingCB );
        viewVar->Set( m_viewCB );
    }

    //-----------------------------------------------------------------------------
    void LightingPass::SetupSRB()
    {
        m_PSO->CreateShaderResourceBinding( &m_SRB, true );
        if ( !m_SRB )
            FATAL_ERROR( "Failed to create lighting pass binding", cmd::RENDERING );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void LightingPass::UpdateLights()
    {
        auto context = m_render->GetContext();

        MapHelper<CBLightsData> lightsData{
            context, m_lightingCB, MAP_WRITE, MAP_FLAG_DISCARD
        };

        std::uint32_t activeLightIndex = 0;
        auto& lights = LightManager::Get().GetLights();
        for ( const auto& lightPtr: lights )
        {
            if ( activeLightIndex >= MAX_LIGHTS )
                continue;

            auto light = lightPtr.get();
            if ( !light )
                continue;

            if ( !light->IsEnabled() )
                continue;

            auto& transform = light->GetTransform();

            auto type = light->GetType();
            auto& dataLight = lightsData->Lights[ activeLightIndex ];

            dataLight.Type = type;
            dataLight.Color = light->GetColor();
            dataLight.Brightness = light->GetBrightness();
            dataLight.Radius = light->GetRadius();
            dataLight.Pos = transform.Position;
            dataLight.ShadowMapIndex = -1;
            dataLight.ShadowSoftness = light->GetShadowSoftness();

            if ( dataLight.Type == LIGHT_SPOT || dataLight.Type == LIGHT_DIRECTIONAL )
            {
                dataLight.Dir = transform.Forward();
                dataLight.View = glm::transpose( light->GetViewMatrix() );
                dataLight.Projection = glm::transpose( light->GetProjectionMatrix() );
            }

            if ( dataLight.Type == LIGHT_SPOT )
            {
                dataLight.OuterCutoff = light->GetOuterCutoff();
                dataLight.InnerCutoff = light->GetInnerCutoff();
            }

            activeLightIndex++;
        }

        lightsData->LightCount = activeLightIndex;
    }

    void LightingPass::UpdateView( const Camera* camera )
    {
        auto context = m_render->GetContext();

        MapHelper<CBViewData> viewData{
            context, m_viewCB, MAP_WRITE, MAP_FLAG_DISCARD
        };

        const auto viewMat = glm::transpose( camera->GetViewMatrix() );

        viewData->View = viewMat;
        viewData->InvView = glm::inverse( viewMat );
    }
}

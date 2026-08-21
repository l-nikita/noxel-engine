#include <Graphics/GraphicsTools/interface/MapHelper.hpp>

#include "renderer/deferred/passes/lightingpass.hpp"
#include "iengine.hpp"
#include "player.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "dbg/debug.hpp"
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
    }

    //-----------------------------------------------------------------------------
    void LightingPass::Render( const LightingPassContext& ctx )
    {
        auto context = m_render->GetContext();

        auto gBuffer = ctx.GBuffer;
        auto camera = ctx.Camera;

        context->SetPipelineState( m_PSO );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tColor" )
                ->Set( gBuffer->GetColor().SRV );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tNormal" )
                ->Set( gBuffer->GetNormal().SRV );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tPosition" )
                ->Set( gBuffer->GetPosition().SRV );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tSpecular" )
                ->Set( gBuffer->GetSpecular().SRV );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tCubemap" )
                ->Set( ctx.SkyTextureView );

        UpdateShadows();
        UpdateLights();
        UpdateView( camera );

        context->CommitShaderResources(
            m_SRB,
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );

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
            FATAL_ERROR( "Failed to create SkyPass binding", cmd::RENDERING );
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
        const auto& lights = m_pipeline->GetLightManager()->GetLights();
        for ( const auto& [ light, shadowMap ]: lights )
        {
            if ( activeLightIndex >= lighting::MAX_LIGHTS )
                continue;

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
            dataLight.ShadowMapIndex = GetShadowSlot( light.get() );
            dataLight.ShadowSoftness = light->GetShadowSoftness();

            if ( dataLight.Type == lighting::LIGHT_SPOT
                 || dataLight.Type == lighting::LIGHT_DIRECTIONAL )
            {
                dataLight.Dir = transform.Forward();
                dataLight.View = glm::transpose( light->GetViewMatrix() );
                dataLight.Projection = glm::transpose( light->GetProjectionMatrix() );
            }

            if ( dataLight.Type == lighting::LIGHT_SPOT )
            {
                dataLight.OuterCutoff = light->GetOuterCutoff();
                dataLight.InnerCutoff = light->GetInnerCutoff();
            }

            activeLightIndex++;
        }

        lightsData->LightCount = activeLightIndex;
    }

    //-----------------------------------------------------------------------------
    void LightingPass::UpdateShadows()
    {
        m_shadowSlots.fill( nullptr );

        std::array<const lighting::LightData*, lighting::MAX_SHADOWS_MAPS> slots{};
        std::uint32_t slotCount = 0;
        for ( const auto& data: m_pipeline->GetLightManager()->GetLights() )
        {
            if ( slotCount >= lighting::MAX_SHADOWS_MAPS )
                break;

            if ( !data.Light || !data.ShadowMap || !data.ShadowMap->GetSRV() )
                continue;

            if ( !data.Light->IsA( lighting::LIGHT_SPOT )
                 && !data.Light->IsA( lighting::LIGHT_DIRECTIONAL ) )
                continue;

            if ( !data.Light->IsEnabled() || !data.Light->IsShadowsEnabled() )
                continue;

            m_shadowSlots[ slotCount ] = data.Light.get();
            slots[ slotCount++ ] = &data;
        }

        static constexpr const char* SHADOW_VARS[ ] = {
            "g_tShadowMap0", "g_tShadowMap1", "g_tShadowMap2", "g_tShadowMap3"
        };

        for ( std::size_t i = 0; i < lighting::MAX_SHADOWS_MAPS; ++i )
        {
            ITextureView* srv = slots[ i ]
                                    ? slots[ i ]->ShadowMap->GetSRV()
                                    : m_render->GetDefaultTextures().Black->GetDefaultView(
                                        TEXTURE_VIEW_SHADER_RESOURCE );

            m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, SHADOW_VARS[ i ] )
                    ->Set( srv );
        }
    }

    //-----------------------------------------------------------------------------
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

    int LightingPass::GetShadowSlot( const lighting::Light* lightA ) const
    {
        for ( std::size_t i = 0; i < m_shadowSlots.size(); ++i )
        {
            if ( m_shadowSlots[ i ] == lightA )
                return i;
        }

        return -1;
    }
}

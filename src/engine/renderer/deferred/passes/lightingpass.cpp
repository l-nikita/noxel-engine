#include <Graphics/GraphicsTools/interface/MapHelper.hpp>

#include "renderer/deferred/passes/lightingpass.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "dbg/debug.hpp"
#include "renderer/factory.hpp"
#include "renderer/shader_cbuffers.hpp"

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
        auto swapChain = m_render->GetSwapChain();
        auto gbuffer = ctx.GBuffer;

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

        auto lightingVar =
                m_PSO->GetStaticVariableByName( SHADER_TYPE_PIXEL, "LightsData" );

        if ( !lightingVar )
            FATAL_ERROR( "Lighting shader constant buffer was not found", cmd::RENDERING );

        lightingVar->Set( m_lightingCB );
    }

    //-----------------------------------------------------------------------------
    void LightingPass::SetupSRB()
    {
        m_PSO->CreateShaderResourceBinding( &m_SRB, true );
        if ( !m_SRB )
            FATAL_ERROR( "Failed to create lighting pass binding", cmd::RENDERING );
    }
}

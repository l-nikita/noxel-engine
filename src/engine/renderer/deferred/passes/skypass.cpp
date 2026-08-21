#include <Graphics/GraphicsTools/interface/MapHelper.hpp>

#include "renderer/deferred/passes/skypass.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "dbg/debug.hpp"
#include "renderer/factory.hpp"
#include "renderer/meshbuilder.hpp"
#include "renderer/shader_cbuffers.hpp"

namespace render::deferred::passes
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SkyPass::SkyPass( const DeferredPassContext& ctx )
        : DeferredPass( ctx )
    {
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void SkyPass::Setup()
    {
        SetupPSO();
        SetupConstantBuffers();
        SetupSRB();
        SetupCubemap();
    }

    //-----------------------------------------------------------------------------
    void SkyPass::Render( const SkyPassContext& ctx )
    {
        auto context = m_render->GetContext();
        auto camera = ctx.Camera;

        context->SetPipelineState( m_PSO );

        m_SRB->GetVariableByName( SHADER_TYPE_PIXEL, "g_tSkyTexture" )
                ->Set( ctx.SkyTextureView );

        context->CommitShaderResources(
            m_SRB,
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );

        UpdateTransform( camera );

        m_cubeModel->DrawPart( context, 0 );
    }

    //-----------------------------------------------------------------------------
    void SkyPass::SetupPSO()
    {
        auto swapChain = m_render->GetSwapChain();
        auto factory = m_render->GetFactory();

        GraphicsPipelineStateCreateInfo info;
        info.PSODesc.Name = "Sky Pass";
        info.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        //-----------------------------------------------------------------------------
        auto& gp = info.GraphicsPipeline;

        gp.NumRenderTargets = 1;
        gp.RTVFormats[ 0 ] = swapChain->GetDesc().ColorBufferFormat;
        gp.DSVFormat = TEX_FORMAT_D24_UNORM_S8_UINT;
        gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        gp.DepthStencilDesc.DepthEnable = True;
        gp.DepthStencilDesc.DepthWriteEnable = False;
        gp.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
        gp.RasterizerDesc.CullMode = CULL_MODE_FRONT;
        gp.RasterizerDesc.FillMode = FILL_MODE_SOLID;

        //-----------------------------------------------------------------------------
        auto skyPass = g_Assets->FindAsset( "shaders/skypass.nshader", assets::Type::SHADER );
        if ( !skyPass )
            FATAL_ERROR( "SkyPass shader not found", cmd::RENDERING );

        m_shaderData = g_Assets->LoadData<assets::ShaderData>( skyPass.value() );
        assert( m_shaderData );

        auto [ vs, ps ] = factory->CreateShader( skyPass.value() );
        info.pVS = vs;
        info.pPS = ps;

        factory->CreatePipelineState( info, *m_shaderData, &m_PSO );
    }

    //-----------------------------------------------------------------------------
    void SkyPass::SetupConstantBuffers()
    {
        auto device = m_render->GetDevice();

        BufferDesc cbTransDesc;
        cbTransDesc.Name = "SkyPass Transform Constant Buffer";
        cbTransDesc.Size = sizeof( CBTransform );
        cbTransDesc.Usage = USAGE_DYNAMIC;
        cbTransDesc.BindFlags = BIND_UNIFORM_BUFFER;
        cbTransDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer( cbTransDesc, nullptr, &m_transformCB );

        auto transVar =
                m_PSO->GetStaticVariableByName( SHADER_TYPE_VERTEX, "Transform" );

        if ( !transVar )
            FATAL_ERROR( "Sky shader constant buffer was not found", cmd::RENDERING );

        transVar->Set( m_transformCB );
    }

    //-----------------------------------------------------------------------------
    void SkyPass::SetupSRB()
    {
        m_PSO->CreateShaderResourceBinding( &m_SRB, true );
        if ( !m_SRB )
            FATAL_ERROR( "Failed to create SkyPass binding", cmd::RENDERING );
    }

    //-----------------------------------------------------------------------------
    void SkyPass::SetupCubemap()
    {
        auto builder = m_render->GetBuilder();
        auto cube = builder->CreateCube();

        m_cubeMesh = std::make_unique<Mesh>( m_render->GetDevice(), cube );
        m_cubeModel = std::make_unique<ModelRenderer>( m_cubeMesh.get() );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void SkyPass::UpdateTransform( const Camera* camera )
    {
        auto context = m_render->GetContext();

        MapHelper<CBTransform> transformData{
            context, m_transformCB, MAP_WRITE, MAP_FLAG_DISCARD
        };

        glm::mat4 view = glm::mat4( glm::mat3( camera->GetViewMatrix() ) );

        transformData->Model = glm::transpose( glm::mat4( 1.0f ) );
        transformData->View = glm::transpose( view );
        transformData->Projection = glm::transpose( camera->GetProjectionMatrix() );
    }
}

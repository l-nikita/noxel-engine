#include <Graphics/GraphicsTools/interface/MapHelper.hpp>

#include "renderer/deferred/passes/shadowpass.hpp"
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
    ShadowPass::ShadowPass( const DeferredPassContext& ctx )
        : DeferredPass( ctx )
    {
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void ShadowPass::Setup()
    {
        SetupPSO();
        SetupConstantBuffers();
        SetupSRB();
    }

    //-----------------------------------------------------------------------------
    void ShadowPass::Render( const ShadowPassContext& ctx )
    {
        auto context = m_render->GetContext();

        context->SetPipelineState( m_PSO );

        context->CommitShaderResources(
            m_SRB,
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );

        for ( auto model: ctx.ModelRenderers )
        {
            if ( !model )
                continue;

            ApplyTransform( model->GetTransformMatrix(), ctx.Light );

            for ( std::size_t i = 0; i < model->GetPartCount(); ++i )
                model->DrawPart( context, i );
        }
    }

    //-----------------------------------------------------------------------------
    void ShadowPass::SetupPSO()
    {
        auto factory = m_render->GetFactory();

        GraphicsPipelineStateCreateInfo info;
        info.PSODesc.Name = "Shadow Pass";
        info.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        //-----------------------------------------------------------------------------
        auto& gp = info.GraphicsPipeline;

        gp.NumRenderTargets = 0;
        gp.RTVFormats[ 0 ] = TEX_FORMAT_UNKNOWN;
        gp.DSVFormat = TEX_FORMAT_D24_UNORM_S8_UINT;
        gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        gp.DepthStencilDesc.DepthEnable = True;
        gp.DepthStencilDesc.DepthWriteEnable = True;
        gp.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS;

        static constexpr LayoutElement elements[ ] = {
            {"ATTRIB", 0, 0, 3, VT_FLOAT32, False, offsetof( Vertex, Position ), sizeof( Vertex )},
        };

        static_assert( std::is_standard_layout_v<Vertex> );

        gp.InputLayout.LayoutElements = elements;
        gp.InputLayout.NumElements = std::size( elements );

        //-----------------------------------------------------------------------------
        auto shadowPass = g_Assets->FindAsset( "shaders/shadowpass.nshader", assets::Type::SHADER );
        if ( !shadowPass )
            FATAL_ERROR( "ShadowPass shader not found", cmd::RENDERING );

        m_shaderData = g_Assets->LoadData<assets::ShaderData>( shadowPass.value() );
        assert( m_shaderData );

        auto [ vs, ps ] = factory->CreateShader( shadowPass.value() );
        info.pVS = vs;
        info.pPS = ps;

        factory->CreatePipelineState( info, *m_shaderData, &m_PSO );
    }

    //-----------------------------------------------------------------------------
    void ShadowPass::SetupConstantBuffers()
    {
        auto device = m_render->GetDevice();

        BufferDesc cbTransDesc;
        cbTransDesc.Name = "ShadowPass Transform Constant Buffer";
        cbTransDesc.Size = sizeof( CBTransform );
        cbTransDesc.Usage = USAGE_DYNAMIC;
        cbTransDesc.BindFlags = BIND_UNIFORM_BUFFER;
        cbTransDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer( cbTransDesc, nullptr, &m_transformCB );

        //-----------------------------------------------------------------------------
        auto transformVar =
                m_PSO->GetStaticVariableByName( SHADER_TYPE_VERTEX, "Transform" );

        if ( !transformVar )
            FATAL_ERROR( "ShadowPass shader constant buffer was not found", cmd::RENDERING );

        transformVar->Set( m_transformCB );
    }

    void ShadowPass::SetupSRB()
    {
        m_PSO->CreateShaderResourceBinding( &m_SRB, true );
        if ( !m_SRB )
            FATAL_ERROR( "Failed to create ShadowPass binding", cmd::RENDERING );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void ShadowPass::ApplyTransform( const glm::mat4& modelMat, const lighting::Light* light )
    {
        auto context = m_render->GetContext();

        MapHelper<CBTransform> transformData{
            context, m_transformCB, MAP_WRITE, MAP_FLAG_DISCARD
        };

        const auto viewMat = light->GetViewMatrix();
        const auto modelView = viewMat * modelMat;

        transformData->Model = glm::transpose( modelMat );
        transformData->View = glm::transpose( viewMat );
        transformData->Projection = glm::transpose( light->GetProjectionMatrix() );

        transformData->ModelView = glm::transpose( modelView );
        transformData->InvModelView = glm::inverse( modelView );
    }
}

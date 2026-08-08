#include <Graphics/GraphicsTools/interface/MapHelper.hpp>

#include "renderer/deferred/geometrypass.hpp"

#include <ranges>

#include "assetsystem/iassetsystem.hpp"
#include "dbg/debug.hpp"
#include "renderer/factory.hpp"
#include "renderer/shader_cbuffers.hpp"
#include "renderer/vertex.hpp"
#include "renderer/deferred/deferred_render_pipeline.hpp"

namespace render::deferred
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    GeometryPass::GeometryPass( RenderSystem* render, DeferredRenderPipeline* pipeline )
        : m_render( render ), m_pipeline( pipeline )
    {
    }

    //-----------------------------------------------------------------------------
    GeometryPass::~GeometryPass()
    {
        m_geometryBindings.clear();

        m_matFactorsCB.Release();
        m_tintCB.Release();
        m_transformCB.Release();
        m_geometryPSO.Release();
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void GeometryPass::Setup()
    {
        SetupPSO();
        SetupConstantBuffers();
    }

    //-----------------------------------------------------------------------------
    void GeometryPass::SetupPSO()
    {
        auto device = m_render->GetDevice();

        GraphicsPipelineStateCreateInfo info;
        info.PSODesc.Name = "Geometry Pass";
        info.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        //-----------------------------------------------------------------------------
        auto& gp = info.GraphicsPipeline;

        gp.NumRenderTargets = 4;
        gp.RTVFormats[ 0 ] = TEX_FORMAT_RGBA8_UNORM;
        gp.RTVFormats[ 1 ] = TEX_FORMAT_RGBA8_UNORM;
        gp.RTVFormats[ 2 ] = TEX_FORMAT_RGBA8_UNORM;
        gp.RTVFormats[ 3 ] = TEX_FORMAT_RGBA8_UNORM;
        gp.DSVFormat = TEX_FORMAT_D24_UNORM_S8_UINT;

        gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        gp.RasterizerDesc.FillMode = FILL_MODE_SOLID;
        gp.RasterizerDesc.CullMode = CULL_MODE_BACK;
        gp.RasterizerDesc.FrontCounterClockwise = False;

        gp.DepthStencilDesc.DepthEnable = True;
        gp.DepthStencilDesc.DepthWriteEnable = True;
        gp.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS;

        //-----------------------------------------------------------------------------
        static const LayoutElement elements[ ] = {
            {"ATTRIB", 0, 0, 3, VT_FLOAT32, False, offsetof( Vertex, Position ), sizeof( Vertex )},
            {"ATTRIB", 1, 0, 3, VT_FLOAT32, False, offsetof( Vertex, Normal ), sizeof( Vertex )},
            {"ATTRIB", 2, 0, 3, VT_FLOAT32, False, offsetof( Vertex, Tangent ), sizeof( Vertex )},
            {"ATTRIB", 3, 0, 3, VT_FLOAT32, False, offsetof( Vertex, Bitangent ), sizeof( Vertex )},
            {"ATTRIB", 4, 0, 2, VT_FLOAT32, False, offsetof( Vertex, UV ), sizeof( Vertex )},
            {"ATTRIB", 5, 0, 4, VT_FLOAT32, False, offsetof( Vertex, Color ), sizeof( Vertex )},
        };

        static_assert( std::is_standard_layout_v<Vertex> );

        gp.InputLayout.LayoutElements = elements;
        gp.InputLayout.NumElements = std::size( elements );

        auto geometryPass = g_Assets->GetAsset( "shaders/geometrypass.nshader", assets::Type::SHADER );
        if ( !geometryPass )
            FATAL_ERROR( "GeometryPass shader not found", cmd::RENDERING );

        m_shaderData = g_Assets->LoadData<assets::ShaderData>( geometryPass.value() );

        auto [ vs, ps ] = Factory::CreateShader( geometryPass.value() );

        info.pVS = vs;
        info.pPS = ps;

        //-----------------------------------------------------------------------------
        std::vector<ShaderResourceVariableDesc> vars;

        for ( auto& texParam: m_shaderData->Textures | std::views::values )
        {
            auto type = texParam.VarType;
            auto srvType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;

            if ( type == "STATIC" )
                srvType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
            else if ( type == "DYNAMIC" )
                srvType = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;

            vars.emplace_back( SHADER_TYPE_PIXEL, texParam.VarBind.c_str(), srvType );
        }

        info.PSODesc.ResourceLayout.Variables = vars.data();
        info.PSODesc.ResourceLayout.NumVariables = static_cast<std::uint32_t>(vars.size());

        std::vector<ImmutableSamplerDesc> immutableSamplers;

        for ( auto& [ samBindName, sampParam ]: m_shaderData->Samplers )
        {
            SamplerDesc sampler;

            auto minFilter = sampParam.MinFilter;
            auto magFilter = sampParam.MagFilter;
            auto mipFilter = sampParam.MipFilter;

            auto addressU = sampParam.AddressU;
            auto addressV = sampParam.AddressV;
            auto addressW = sampParam.AddressW;

            if ( minFilter == "LINEAR" )
                sampler.MinFilter = FILTER_TYPE_LINEAR;
            else if ( minFilter == "POINT" )
                sampler.MinFilter = FILTER_TYPE_POINT;

            if ( magFilter == "LINEAR" )
                sampler.MagFilter = FILTER_TYPE_LINEAR;
            else if ( magFilter == "POINT" )
                sampler.MagFilter = FILTER_TYPE_POINT;

            if ( mipFilter == "LINEAR" )
                sampler.MipFilter = FILTER_TYPE_LINEAR;
            else if ( mipFilter == "POINT" )
                sampler.MipFilter = FILTER_TYPE_POINT;

            if ( addressU == "WARP" )
                sampler.AddressU = TEXTURE_ADDRESS_WRAP;
            else if ( addressU == "CLAMP" )
                sampler.AddressU = TEXTURE_ADDRESS_CLAMP;

            if ( addressV == "WARP" )
                sampler.AddressV = TEXTURE_ADDRESS_WRAP;
            else if ( addressV == "CLAMP" )
                sampler.AddressV = TEXTURE_ADDRESS_CLAMP;

            if ( addressW == "WARP" )
                sampler.AddressW = TEXTURE_ADDRESS_WRAP;
            else if ( addressW == "CLAMP" )
                sampler.AddressW = TEXTURE_ADDRESS_CLAMP;

            immutableSamplers.emplace_back( SHADER_TYPE_PIXEL, samBindName.c_str(), sampler );
        }

        if ( immutableSamplers.empty() )
            FATAL_ERROR( "No immutable samplers for geometry pass found", cmd::RENDERING );

        info.PSODesc.ResourceLayout.ImmutableSamplers = immutableSamplers.data();
        info.PSODesc.ResourceLayout.NumImmutableSamplers = static_cast<std::uint32_t>(immutableSamplers.size());

        device->CreateGraphicsPipelineState( info, &m_geometryPSO );
    }

    //-----------------------------------------------------------------------------
    void GeometryPass::SetupConstantBuffers()
    {
        auto device = m_render->GetDevice();

        BufferDesc cbTransDesc;
        cbTransDesc.Name = "Geometry Transform Constant Buffer";
        cbTransDesc.Size = sizeof( CBTransform );
        cbTransDesc.Usage = USAGE_DYNAMIC;
        cbTransDesc.BindFlags = BIND_UNIFORM_BUFFER;
        cbTransDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer( cbTransDesc, nullptr, &m_transformCB );

        BufferDesc cbTintDesc;
        cbTintDesc.Name = "Geometry Tint Constant Buffer";
        cbTintDesc.Size = sizeof( CBTint );
        cbTintDesc.Usage = USAGE_DYNAMIC;
        cbTintDesc.BindFlags = BIND_UNIFORM_BUFFER;
        cbTintDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer( cbTintDesc, nullptr, &m_tintCB );

        BufferDesc cbMatFactorsDesc;
        cbMatFactorsDesc.Name = "Geometry Material Factors Constant Buffer";
        cbMatFactorsDesc.Size = sizeof( CBMaterialFactors );
        cbMatFactorsDesc.Usage = USAGE_DYNAMIC;
        cbMatFactorsDesc.BindFlags = BIND_UNIFORM_BUFFER;
        cbMatFactorsDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer( cbMatFactorsDesc, nullptr, &m_matFactorsCB );

        if ( !m_transformCB || !m_tintCB || !m_matFactorsCB )
            FATAL_ERROR( "Failed to create geometry constant buffers", cmd::RENDERING );

        //-----------------------------------------------------------------------------
        auto transformVar =
                m_geometryPSO->GetStaticVariableByName( SHADER_TYPE_VERTEX, "Transform" );

        auto tintVar =
                m_geometryPSO->GetStaticVariableByName( SHADER_TYPE_PIXEL, "Tint" );

        auto matFactorsVar =
                m_geometryPSO->GetStaticVariableByName( SHADER_TYPE_PIXEL, "MaterialFactors" );

        if ( !transformVar || !tintVar || !matFactorsVar )
            FATAL_ERROR( "Geometry shader constant buffer was not found", cmd::RENDERING );

        transformVar->Set( m_transformCB );
        tintVar->Set( m_tintCB );
        matFactorsVar->Set( m_matFactorsCB );
    }

    IShaderResourceBinding* GeometryPass::GetGeometryBinding( const Material& material )
    {
        if ( const auto it = m_geometryBindings.find( &material ); it != m_geometryBindings.end() )
            return it->second;

        RefCntAutoPtr<IShaderResourceBinding> binding;
        m_geometryPSO->CreateShaderResourceBinding( &binding, true );
        if ( !binding )
            FATAL_ERROR( "Failed to create geometry material binding", cmd::RENDERING );

        const auto& defTextures = m_render->GetDefaultTextures();

        for ( const auto& [ texName, texParam ]: m_shaderData->Textures )
        {
            auto tex = material.GetTexture( texName );
            auto var = binding->GetVariableByName( SHADER_TYPE_PIXEL, texParam.VarBind.c_str() );

            if ( !var )
                FATAL_ERROR( std::string( "Geometry shader resource is missing: " ) + texName, cmd::RENDERING );

            RefCntAutoPtr<ITexture> defTex;
            auto defTexStr = texParam.DefaultTexture;

            if ( defTexStr == "white" )
                defTex = defTextures.White;
            else if ( defTexStr == "black" )
                defTex = defTextures.Black;
            else if ( defTexStr == "neutral_normal" )
                defTex = defTextures.NeutralNormal;
            else
                FATAL_ERROR( "Default texture for geometry shader is invalid: " + defTexStr, cmd::RENDERING );

            if ( !tex )
                tex = defTex.RawPtr();

            var->Set( tex->GetDefaultView( TEXTURE_VIEW_SHADER_RESOURCE ) );
        }

        auto [ it, inserted ] = m_geometryBindings.emplace( &material, std::move( binding ) );
        return it->second;
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void GeometryPass::Render( const GeometryPassContext& ctx )
    {
        auto context = m_render->GetContext();
        auto gbuffer = ctx.GBuffer;

        ITextureView* rtvs[ ] = {
            gbuffer->GetColor().RTV,
            gbuffer->GetNormal().RTV,
            gbuffer->GetPosition().RTV,
            gbuffer->GetSpecular().RTV
        };

        context->SetRenderTargets( std::size( rtvs ), rtvs, gbuffer->GetDSV(),
                                   RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        for ( auto rtv: rtvs )
            context->ClearRenderTarget( rtv, CLEAR_COLOR, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        context->ClearDepthStencil( gbuffer->GetDSV(), CLEAR_DEPTH_FLAG, 1.0f, 0,
                                    RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        context->SetPipelineState( m_geometryPSO );

        {
            MapHelper<CBTint> tintData{context, m_tintCB, MAP_WRITE, MAP_FLAG_DISCARD};
            tintData->Tint = glm::vec4( 1.0f );
            tintData->_pad0 = 0.0f;
            tintData->_pad1 = 0.0f;
            tintData->_pad2 = 0.0f;
            tintData->Emission = 0.0f;
        }

        for ( const auto& model: ctx.ModelRenderers )
        {
            if ( !model )
                continue;

            ApplyTranform( model->GetTransformMatrix(), ctx.Camera );

            for ( std::size_t partIndex = 0; partIndex < model->GetPartCount(); ++partIndex )
            {
                const auto* part = model->GetPart( partIndex );
                const Material& material = *part->MaterialResource;

                ApplyMaterialParameters( material );

                auto binding = GetGeometryBinding( material );
                context->CommitShaderResources( binding, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
                model->DrawPart( context, partIndex );
            }
        }
    }

    //-----------------------------------------------------------------------------
    void GeometryPass::ApplyTranform( const glm::mat4& modelMatrix, const Camera* camera )
    {
        {
            auto context = m_render->GetContext();

            MapHelper<CBTransform> transformData{
                context, m_transformCB, MAP_WRITE, MAP_FLAG_DISCARD
            };
            transformData->Model = glm::transpose( modelMatrix );
            transformData->View = glm::transpose( camera->GetViewMatrix() );
            transformData->Projection = glm::transpose( camera->GetProjectionMatrix() );
        }
    }

    //-----------------------------------------------------------------------------
    void GeometryPass::ApplyMaterialParameters( const Material& material )
    {
        auto context = m_render->GetContext();
        CBMaterialFactors factors{};

        if ( material.GetTexture( "color" ) )
            factors.TextureMask |= MATERIAL_HAS_COLOR_MAP;

        if ( material.GetTexture( "normal" ) )
            factors.TextureMask |= MATERIAL_HAS_NORMAL_MAP;

        if ( material.GetTexture( "rough" ) )
            factors.TextureMask |= MATERIAL_HAS_ROUGH_MAP;

        if ( material.GetTexture( "metal" ) )
            factors.TextureMask |= MATERIAL_HAS_METAL_MAP;

        if ( material.GetTexture( "ao" ) )
            factors.TextureMask |= MATERIAL_HAS_AO_MAP;

        if ( material.GetTexture( "height" ) )
            factors.TextureMask |= MATERIAL_HAS_HEIGHT_MAP;

        for ( const auto& [ paramName, paramValue ]: material.GetParameters() )
        {
            const auto getFactor = [&]() -> std::optional<float>
            {
                if ( const auto value = std::get_if<float>( &paramValue ) )
                    return *value;

                if ( const auto value = std::get_if<int>( &paramValue ) )
                    return static_cast<float>(*value);

                return std::nullopt;
            };

            if ( paramName == "color" )
            {
                if ( const auto value = getFactor() )
                    factors.ColorFactor = glm::vec4( *value );
            } else if ( paramName == "normal" )
            {
                if ( const auto value = getFactor() )
                    factors.NormalFactor = *value;
            } else if ( paramName == "rough" )
            {
                if ( const auto value = getFactor() )
                    factors.RoughFactor = *value;
            } else if ( paramName == "metal" )
            {
                if ( const auto value = getFactor() )
                    factors.MetalFactor = *value;
            } else if ( paramName == "ao" )
            {
                if ( const auto value = getFactor() )
                    factors.AOFactor = *value;
            } else if ( paramName == "height" )
            {
                if ( const auto value = getFactor() )
                    factors.HeightFactor = *value;
            }
        }

        MapHelper<CBMaterialFactors> mappedFactors{
            context,
            m_matFactorsCB,
            MAP_WRITE,
            MAP_FLAG_DISCARD
        };

        *mappedFactors = factors;
    }
}

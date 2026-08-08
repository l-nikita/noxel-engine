#include <Graphics/GraphicsEngine/interface/PipelineState.h>

#include "assetsystem/iassetsystem.hpp"
#include "engine/window.hpp"
#include "renderer/deferred_render_pipeline.hpp"

#include "factory.hpp"
#include "dbg/debug.hpp"
#include "renderer/camera.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/vertex.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void CalculateTangents( std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices )
    {
        for ( auto& v: vertices )
        {
            v.Tangent = glm::vec3( 0.0f );
            v.Bitangent = glm::vec3( 0.0f );
        }

        for ( size_t i = 0; i < indices.size(); i += 3 )
        {
            Vertex& v0 = vertices[ indices[ i + 0 ] ];
            Vertex& v1 = vertices[ indices[ i + 1 ] ];
            Vertex& v2 = vertices[ indices[ i + 2 ] ];

            glm::vec3 edge1 = v1.Position - v0.Position;
            glm::vec3 edge2 = v2.Position - v0.Position;

            glm::vec2 deltaUV1 = v1.UV - v0.UV;
            glm::vec2 deltaUV2 = v2.UV - v0.UV;

            float f = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y );

            glm::vec3 tangent;
            glm::vec3 bitangent;

            tangent.x = f * ( deltaUV2.y * edge1.x - deltaUV1.y * edge2.x );
            tangent.y = f * ( deltaUV2.y * edge1.y - deltaUV1.y * edge2.y );
            tangent.z = f * ( deltaUV2.y * edge1.z - deltaUV1.y * edge2.z );

            bitangent.x = f * ( -deltaUV2.x * edge1.x + deltaUV1.x * edge2.x );
            bitangent.y = f * ( -deltaUV2.x * edge1.y + deltaUV1.x * edge2.y );
            bitangent.z = f * ( -deltaUV2.x * edge1.z + deltaUV1.x * edge2.z );

            v0.Tangent += tangent;
            v1.Tangent += tangent;
            v2.Tangent += tangent;
            v0.Bitangent += bitangent;
            v1.Bitangent += bitangent;
            v2.Bitangent += bitangent;
        }

        for ( auto& v: vertices )
        {
            v.Tangent = glm::normalize( v.Tangent - v.Normal * glm::dot( v.Normal, v.Tangent ) );
            v.Bitangent = glm::normalize( glm::cross( v.Normal, v.Tangent ) );
        }
    }

    constexpr float CLEAR_COLOR[ ] = {0.1f, 0.2f, 0.3f, 1.0f};

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SysInit DeferredRenderPipeline::Initialize()
    {
        auto window = g_Render->GetWindow();
        auto [ wW, wH ] = window->GetSize();

        SetupGBuffer( wW, wH );
        SetupGeometryPass();

        window->SubscribeResize( [this]( const WindowSize& size )
        {
            auto [ newWW, newWH ] = size;
            SetupGBuffer( newWW, newWH );
        } );

        return {};
    }

    void DeferredRenderPipeline::Shutdown()
    {
        m_GBuffer.Release();
    }

    void DeferredRenderPipeline::ClearBackBuffer()
    {
        auto swapChain = g_Render->GetSwapChain();
        auto context = g_Render->GetContext();

        ITextureView* bBuffer = swapChain->GetCurrentBackBufferRTV();
        ITextureView* bBufferDepth = swapChain->GetDepthBufferDSV();
        context->SetRenderTargets( 1, &bBuffer, bBufferDepth, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        context->ClearRenderTarget( bBuffer, CLEAR_COLOR, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
        context->ClearDepthStencil( bBufferDepth, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::Render()
    {
        ClearBackBuffer();
        DoGeometryPass();

        g_Render->Present();
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::DoGeometryPass() const
    {
        auto context = g_Render->GetContext();

        ITextureView* rtvs[ ] = {
            m_GBuffer.Color.RTV,
            m_GBuffer.Normal.RTV,
            m_GBuffer.Position.RTV,
            m_GBuffer.Specular.RTV
        };

        context->SetRenderTargets( std::size( rtvs ), rtvs, m_GBuffer.DSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        for ( auto rtv: rtvs )
            context->ClearRenderTarget( rtv, CLEAR_COLOR, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        context->ClearDepthStencil( m_GBuffer.DSV, CLEAR_DEPTH_FLAG, 1.0f, 0,
                                    RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        context->SetPipelineState( m_geometryPSO );
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::DoShadowPass() const
    {
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::DoLightingPass() const
    {
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::DoSkyboxPass() const
    {
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::SetupGBuffer( std::uint32_t width, std::uint32_t height )
    {
        auto device = g_Render->GetDevice();
        auto context = g_Render->GetContext();

        context->SetRenderTargets( 0, nullptr, nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        m_GBuffer.Release();

        const auto createRenderTarget = [&]( RenderTarget& rt, const char* name )
        {
            TextureDesc desc;
            desc.Name = name;
            desc.Type = RESOURCE_DIM_TEX_2D;
            desc.Width = width;
            desc.Height = height;
            desc.Format = TEX_FORMAT_RGBA32_FLOAT;
            desc.MipLevels = 1;
            desc.BindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
            desc.Usage = USAGE_DEFAULT;
            desc.ClearValue.SetColor( desc.Format, CLEAR_COLOR );

            device->CreateTexture( desc, nullptr, &rt.Texture );

            if ( !rt.Texture )
                FATAL_ERROR( std::string( "Failed to create " ) + name, cmd::RENDERING );

            rt.RTV = rt.Texture->GetDefaultView( TEXTURE_VIEW_RENDER_TARGET );
            rt.SRV = rt.Texture->GetDefaultView( TEXTURE_VIEW_SHADER_RESOURCE );
        };

        createRenderTarget( m_GBuffer.Color, "GBuffer Color" );
        createRenderTarget( m_GBuffer.Normal, "GBuffer Normal" );
        createRenderTarget( m_GBuffer.Position, "GBuffer Position" );
        createRenderTarget( m_GBuffer.Specular, "GBuffer Specular" );

        TextureDesc depthDesc;
        depthDesc.Name = "GBuffer Depth";
        depthDesc.Type = RESOURCE_DIM_TEX_2D;
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.Format = TEX_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.MipLevels = 1;
        depthDesc.BindFlags = BIND_DEPTH_STENCIL;
        depthDesc.Usage = USAGE_DEFAULT;

        depthDesc.ClearValue.SetDepthStencil( depthDesc.Format, 1.0f, 0 );
        device->CreateTexture( depthDesc, nullptr, &m_GBuffer.Depth );

        if ( !m_GBuffer.Depth )
            FATAL_ERROR( "Failed to create GBuffer Depth", cmd::RENDERING );

        m_GBuffer.DSV = m_GBuffer.Depth->GetDefaultView( TEXTURE_VIEW_DEPTH_STENCIL );
    }

    void DeferredRenderPipeline::SetupGeometryPass()
    {
        auto swapChain = g_Render->GetSwapChain();
        auto device = g_Render->GetDevice();

        GraphicsPipelineStateCreateInfo info;
        info.PSODesc.Name = "Geometry Pass";
        info.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        auto& gp = info.GraphicsPipeline;

        gp.NumRenderTargets = 4;
        gp.RTVFormats[ 0 ] = TEX_FORMAT_RGBA32_FLOAT;
        gp.RTVFormats[ 1 ] = TEX_FORMAT_RGBA32_FLOAT;
        gp.RTVFormats[ 2 ] = TEX_FORMAT_RGBA32_FLOAT;
        gp.RTVFormats[ 3 ] = TEX_FORMAT_RGBA32_FLOAT;
        gp.DSVFormat = TEX_FORMAT_D24_UNORM_S8_UINT;

        gp.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        gp.RasterizerDesc.FillMode = FILL_MODE_SOLID;
        gp.RasterizerDesc.CullMode = CULL_MODE_BACK;
        gp.RasterizerDesc.FrontCounterClockwise = False;

        gp.DepthStencilDesc.DepthEnable = True;
        gp.DepthStencilDesc.DepthWriteEnable = True;
        gp.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS;

        static const LayoutElement elements[ ] = {
            {
                "ATTRIB", 0, 0, 3, VT_FLOAT32, False,
                offsetof( Vertex, Position ), sizeof( Vertex )
            },

            {
                "ATTRIB", 1, 0, 3, VT_FLOAT32, False,
                offsetof( Vertex, Normal ), sizeof( Vertex )
            },

            {
                "ATTRIB", 2, 0, 3, VT_FLOAT32, False,
                offsetof( Vertex, Tangent ), sizeof( Vertex )
            },

            {
                "ATTRIB", 3, 0, 3, VT_FLOAT32, False,
                offsetof( Vertex, Bitangent ), sizeof( Vertex )
            },

            {
                "ATTRIB", 4, 0, 2, VT_FLOAT32, False,
                offsetof( Vertex, UV ), sizeof( Vertex )
            },

            {
                "ATTRIB", 5, 0, 4, VT_FLOAT32, False,
                offsetof( Vertex, Color ), sizeof( Vertex )
            },
        };

        static_assert( std::is_standard_layout_v<Vertex> );

        gp.InputLayout.LayoutElements = elements;
        gp.InputLayout.NumElements = std::size( elements );

        auto geometryPass = g_Assets->GetAsset( "shaders/geometrypass.nshader", assets::Type::SHADER );
        if ( !geometryPass )
            FATAL_ERROR( "GeometryPass shader not found", cmd::RENDERING );

        auto [ vs, ps ] = Factory::CreateShader( geometryPass.value() );

        info.pVS = vs;
        info.pPS = ps;
        device->CreateGraphicsPipelineState( info, &m_geometryPSO );
    }

    void DeferredRenderPipeline::SetupShadows()
    {
    }

    void DeferredRenderPipeline::SetupSkybox()
    {
    }
}

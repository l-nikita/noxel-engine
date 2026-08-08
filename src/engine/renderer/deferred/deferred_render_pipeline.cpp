#include <glm/matrix.hpp>
#include <Graphics/GraphicsEngine/interface/PipelineState.h>

#include "renderer/deferred/deferred_render_pipeline.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/materialdata.hpp"
#include "dbg/debug.hpp"
#include "engine/iengine.hpp"
#include "engine/window.hpp"
#include "renderer/camera.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/shader_cbuffers.hpp"
#include "renderer/vertex.hpp"

namespace render::deferred
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------

    static ModelRenderer* g_TestModel;

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

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SysInit DeferredRenderPipeline::Initialize()
    {
        auto window = g_Render->GetWindow();
        auto [ wW, wH ] = window->GetSize();

        m_mainCamera = std::make_unique<Camera>( wW, wH );
        m_mainCamera->SetFOV( 95.0f );
        m_mainCamera->SetNear( 0.01f );
        m_mainCamera->SetFar( 100.0f );

        m_GBuffer = std::make_unique<GBuffer>( g_Render, this );
        m_GBuffer->Setup( wW, wH );

        m_geometryPass = std::make_unique<GeometryPass>( g_Render, this );
        m_geometryPass->Setup();

        SetupTestModel();

        window->SubscribeResize( [this]( const WindowSize& size )
        {
            auto [ newWW, newWH ] = size;

            m_GBuffer->Setup( newWW, newWH );
            m_mainCamera->OnFrameBufferResize( newWW, newWH );
        } );

        return {};
    }

    void DeferredRenderPipeline::Shutdown()
    {
        m_models.clear();
        m_testMaterial.reset();
        m_testMesh.reset();

        m_GBuffer.reset();
        m_mainCamera.reset();
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::Render()
    {
        ClearBackBuffer();

        GeometryPassContext gpr{
            .GBuffer = m_GBuffer.get(),
            .Camera = m_mainCamera.get(),
            .ModelRenderers = m_models,
        };

        m_geometryPass->Render( gpr );

        static auto totalTime = 0.0f;
        totalTime += g_Engine->DeltaTime();

        g_TestModel->SetPosition( glm::vec3(
            sin( totalTime ) * 1.0f,
            -3.0f,
            cos( totalTime ) * 1.0f
        ) );

        glm::quat rot = glm::angleAxis( totalTime * 3.0f, glm::normalize( glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
        g_TestModel->SetRotation( rot );

        auto swapChain = g_Render->GetSwapChain();
        auto context = g_Render->GetContext();

        ITextureView* bBuffer = swapChain->GetCurrentBackBufferRTV();

        CopyTextureAttribs copyAttribs;
        copyAttribs.pSrcTexture = m_GBuffer->GetColor().Texture;
        copyAttribs.SrcTextureTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
        copyAttribs.pDstTexture = bBuffer->GetTexture();
        copyAttribs.DstTextureTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        context->CopyTexture( copyAttribs );

        g_Render->Present();
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::AddModel( std::unique_ptr<ModelRenderer> model )
    {
        if ( model )
            m_models.push_back( std::move( model ) );
    }

    Camera* DeferredRenderPipeline::GetCamera() const
    {
        return m_mainCamera.get();
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::SetupTestModel()
    {
        struct CubeFace
        {
            glm::vec3 Normal;
            glm::vec3 Positions[ 4 ];
        };

        const CubeFace faces[ ] = {
            {{0, 0, 1}, {{-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}}},
            {{0, 0, -1}, {{1, -1, -1}, {-1, -1, -1}, {-1, 1, -1}, {1, 1, -1}}},
            {{0, 1, 0}, {{-1, 1, 1}, {1, 1, 1}, {1, 1, -1}, {-1, 1, -1}}},
            {{0, -1, 0}, {{-1, -1, -1}, {1, -1, -1}, {1, -1, 1}, {-1, -1, 1}}},
            {{1, 0, 0}, {{1, -1, 1}, {1, -1, -1}, {1, 1, -1}, {1, 1, 1}}},
            {{-1, 0, 0}, {{-1, -1, -1}, {-1, -1, 1}, {-1, 1, 1}, {-1, 1, -1}}}
        };
        const glm::vec2 uvs[ ] = {
            {0, 1},
            {1, 1},
            {1, 0},
            {0, 0}
        };

        MeshData cubeData;
        cubeData.Vertices.reserve( 24 );
        cubeData.Indices.reserve( 36 );

        for ( const auto& [ Normal, Positions ]: faces )
        {
            const auto firstVertex = static_cast<std::uint32_t>(cubeData.Vertices.size());
            for ( std::size_t i = 0; i < std::size( Positions ); ++i )
            {
                cubeData.Vertices.push_back( Vertex{
                    Positions[ i ], Normal, {}, {}, uvs[ i ], glm::vec4( 1.0f )
                } );
            }

            const std::uint32_t faceIndices[ ] = {0, 1, 2, 2, 3, 0};
            for ( const auto index: faceIndices )
                cubeData.Indices.push_back( firstVertex + index );
        }

        CalculateTangents( cubeData.Vertices, cubeData.Indices );

        const auto metalId = g_Assets->GetAsset( "materials/metal1.nmt", assets::Type::MATERIAL );
        const auto dirtId = g_Assets->GetAsset( "materials/dirt1.nmt", assets::Type::MATERIAL );

        auto metalData = g_Assets->LoadData<assets::MaterialData>( *metalId );
        auto dirtData = g_Assets->LoadData<assets::MaterialData>( *dirtId );

        m_testMesh = std::make_unique<Mesh>( g_Render->GetDevice(), cubeData );
        m_testMaterial = std::make_unique<Material>( *metalData );
        m_testMaterial2 = std::make_unique<Material>( *dirtData );

        //-----------------------------------------------------------------------------
        //auto model = std::make_unique<ModelRenderer>( m_testMesh.get(), m_testMaterial.get() );
        //model->SetPosition( glm::vec3( 0.0f, -2.0f, 5.0f ) );
        //AddModel( std::move( model ) );

        auto model2 = std::make_unique<ModelRenderer>( m_testMesh.get(), m_testMaterial.get() );
        model2->m_Transform.Position = glm::vec3( 0, 0, 5 ) + glm::vec3( 0, -3, -10 );
        AddModel( std::move( model2 ) );

        auto model3 = std::make_unique<ModelRenderer>( m_testMesh.get(), m_testMaterial2.get() );
        model3->m_Transform.Position = glm::vec3( 0, 0, 8 ) + glm::vec3( 0, -3, 7 );
        AddModel( std::move( model3 ) );

        auto model4 = std::make_unique<ModelRenderer>( m_testMesh.get(), m_testMaterial.get() );
        model4->m_Transform.Position = glm::vec3( -5, 0, 8 ) + glm::vec3( 0, -3, -10 );
        AddModel( std::move( model4 ) );

        auto model5 = std::make_unique<ModelRenderer>( m_testMesh.get(), m_testMaterial.get() );
        model5->m_Transform.Scale = glm::vec3( 0.5f );
        g_TestModel = model5.get();

        AddModel( std::move( model5 ) );

        auto floor = std::make_unique<ModelRenderer>( m_testMesh.get(), m_testMaterial.get() );
        floor->m_Transform.Position = glm::vec3( 0, -5, 0 );
        floor->m_Transform.Scale = glm::vec3( 20, 1, 20 );
        AddModel( std::move( floor ) );
    }

    //-----------------------------------------------------------------------------
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
}

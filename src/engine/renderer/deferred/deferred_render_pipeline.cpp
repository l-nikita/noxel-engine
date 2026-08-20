#include <glm/matrix.hpp>
#include <Graphics/GraphicsEngine/interface/PipelineState.h>

#include "renderer/deferred/deferred_render_pipeline.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/materialdata.hpp"
#include "dbg/debug.hpp"
#include "engine/iengine.hpp"
#include "engine/player.hpp"
#include "engine/window.hpp"
#include "renderer/camera.hpp"
#include "renderer/factory.hpp"
#include "renderer/meshbuilder.hpp"
#include "renderer/rendersystem.hpp"

namespace render::deferred
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------

    static ModelRenderer* g_TestModel;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    DeferredRenderPipeline::DeferredRenderPipeline( RenderSystem* render )
        : m_render( render )
    {
        passes::DeferredPassContext ctx{
            .Render = render,
            .Pipeline = this,
        };

        m_geometryPass = std::make_unique<passes::GeometryPass>( ctx );
        m_lightingPass = std::make_unique<passes::LightingPass>( ctx );
        m_skyPass = std::make_unique<passes::SkyPass>( ctx );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SysInit DeferredRenderPipeline::Initialize()
    {
        auto window = m_render->GetWindow();
        auto [ wW, wH ] = window->GetSize();

        m_mainCamera = std::make_unique<Camera>( wW, wH );
        m_mainCamera->SetFOV( 95.0f )
                .SetNear( 0.01f )
                .SetFar( 100.0f );

        m_GBuffer = std::make_unique<GBuffer>( m_render, this );
        m_GBuffer->Setup( wW, wH );

        m_geometryPass->Setup();
        m_lightingPass->Setup();
        m_skyPass->Setup();

        SetupSkybox();
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
        auto swapChain = m_render->GetSwapChain();
        auto context = m_render->GetContext();

        std::vector<ModelRenderer*> models;
        for ( const auto& model: m_models )
        {
            if ( model && !HasFlag( model->m_Flags, flags::NO_DRAW ) )
                models.push_back( model.get() );
        }

        RenderTest();

        ITextureView* bBuffer = swapChain->GetCurrentBackBufferRTV();
        ITextureView* bBufferDepth = swapChain->GetDepthBufferDSV();

        ClearBackBuffer( bBuffer, bBufferDepth );

        //----------------------------------------------
        SetAndClearGBuffer();

        passes::GeometryPassContext gpc{
            .GBuffer = m_GBuffer.get(),
            .Camera = m_mainCamera.get(),
            .ModelRenderers = models,
        };
        m_geometryPass->Render( gpc );

        /*
        auto swapChain = g_Render->GetSwapChain();
        auto context = g_Render->GetContext();

        ITextureView* bBuffer = swapChain->GetCurrentBackBufferRTV();

        CopyTextureAttribs copyAttribs;
        copyAttribs.pSrcTexture = m_GBuffer->GetPosition().Texture;
        copyAttribs.SrcTextureTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
        copyAttribs.pDstTexture = bBuffer->GetTexture();
        copyAttribs.DstTextureTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        context->CopyTexture( copyAttribs );
        */

        //----------------------------------------------
        context->SetRenderTargets(
            1,
            &bBuffer,
            nullptr,
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );

        passes::LightingPassContext lpc{
            .GBuffer = m_GBuffer.get(),
            .Camera = m_mainCamera.get(),
            .SkyTextureView = m_skyboxTexView.RawPtr()
        };
        m_lightingPass->Render( lpc );

        //----------------------------------------------
        context->SetRenderTargets(
            1,
            &bBuffer,
            m_GBuffer->GetDSV(),
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        passes::SkyPassContext spc{
            .Camera = m_mainCamera.get(),
            .SkyTextureView = m_skyboxTexView.RawPtr()
        };

        m_skyPass->Render( spc );

        //----------------------------------------------
        m_render->Present();
    }

    void DeferredRenderPipeline::RenderTest()
    {
        static auto totalTime = 0.0f;
        totalTime += g_Engine->DeltaTime();

        if ( g_Player )
        {
            if ( !g_Player->m_Model )
            {
                auto model = std::make_unique<ModelRenderer>( m_testMesh.get(), m_testMaterial.get() );
                model->SetScale( glm::vec3( 0.3f, 1.0f, 0.3f ) );
                g_Player->m_Model = model.get();

                AddModel( std::move( model ) );
            } else
                g_Player->Draw();
        }

        g_TestModel->SetPosition( glm::vec3(
            sin( totalTime ) * 1.0f,
            -3.0f,
            cos( totalTime ) * 1.0f
        ) );

        glm::quat rot = glm::angleAxis( totalTime * 3.0f, glm::normalize( glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
        g_TestModel->SetRotation( rot );
    }

    Camera* DeferredRenderPipeline::GetCamera() const
    {
        return m_mainCamera.get();
    }

    void DeferredRenderPipeline::SetupSkybox()
    {
        auto factory = m_render->GetFactory();

        auto skyboxMat = g_Assets->FindAsset( "materials/skybox.nmt", assets::Type::MATERIAL );
        if ( !skyboxMat )
            FATAL_ERROR( "Skybox material not found", cmd::RENDERING );

        m_skyboxTex = factory->CreateTextureCube( skyboxMat.value() );
        m_skyboxTexView = m_skyboxTex->GetDefaultView( TEXTURE_VIEW_SHADER_RESOURCE );
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::SetupTestModel()
    {
        const auto metalId = g_Assets->FindAsset( "materials/metal1.nmt", assets::Type::MATERIAL );
        const auto dirtId = g_Assets->FindAsset( "materials/dirt1.nmt", assets::Type::MATERIAL );

        auto metalData = g_Assets->LoadData<assets::MaterialData>( *metalId );
        auto dirtData = g_Assets->LoadData<assets::MaterialData>( *dirtId );

        auto builder = m_render->GetBuilder();

        m_testMesh = std::make_unique<Mesh>( m_render->GetDevice(), builder->CreateCube() );
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
    void DeferredRenderPipeline::AddModel( std::unique_ptr<ModelRenderer> model )
    {
        if ( model )
            m_models.push_back( std::move( model ) );
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::ClearBackBuffer( ITextureView* bBuffer, ITextureView* bBufferDepth )
    {
        auto context = m_render->GetContext();

        context->SetRenderTargets( 1, &bBuffer, bBufferDepth, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        context->ClearRenderTarget( bBuffer, CLEAR_COLOR, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
        context->ClearDepthStencil( bBufferDepth, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
    }

    //-----------------------------------------------------------------------------
    void DeferredRenderPipeline::SetAndClearGBuffer()
    {
        auto context = m_render->GetContext();

        ITextureView* rtvs[ ] = {
            m_GBuffer->GetColor().RTV,
            m_GBuffer->GetNormal().RTV,
            m_GBuffer->GetPosition().RTV,
            m_GBuffer->GetSpecular().RTV
        };

        auto gBufferDSV = m_GBuffer->GetDSV();
        context->SetRenderTargets( std::size( rtvs ), rtvs, gBufferDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        for ( auto rtv: rtvs )
            context->ClearRenderTarget( rtv, CLEAR_COLOR, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        context->ClearDepthStencil( gBufferDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
    }
}

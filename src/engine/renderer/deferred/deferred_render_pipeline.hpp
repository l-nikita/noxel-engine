#pragma once

#include <memory>
#include <vector>

#include "renderer/camera.hpp"
#include "renderer/irenderpipeline.hpp"
#include "renderer/material.hpp"
#include "renderer/mesh.hpp"
#include "renderer/modelrenderer.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/deferred/passes/geometrypass.hpp"
#include "renderer/deferred/passes/lightingpass.hpp"
#include "renderer/deferred/passes/skypass.hpp"
#include "renderer/deferred/gbuffer.hpp"

namespace render::deferred
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class DeferredRenderPipeline final : public IRenderPipeline
    {
    public:
        SYSTEM_METADATA( RENDER_PIPELINE, "DeferredRenderPipeline", SysId::RENDER );

    public:
        DeferredRenderPipeline( RenderSystem* render );

        SysInit Initialize() override;
        void Shutdown() override;

        void Render() override;
        void RenderTest();

        Camera* GetCamera() const override;

    private:
        void SetupSkybox();
        void SetupTestModel();
        void AddModel( std::unique_ptr<ModelRenderer> model );

        void ClearBackBuffer( ITextureView* bBuffer, ITextureView* bBufferDepth );
        void SetAndClearGBuffer();

    private:
        std::unique_ptr<GBuffer> m_GBuffer = {};

        // Passes
        std::unique_ptr<passes::GeometryPass> m_geometryPass;
        std::unique_ptr<passes::LightingPass> m_lightingPass;
        std::unique_ptr<passes::SkyPass> m_skyPass;

        std::unique_ptr<Camera> m_mainCamera = {};

        Diligent::RefCntAutoPtr<ITexture> m_skyboxTex;
        Diligent::RefCntAutoPtr<ITextureView> m_skyboxTexView;

        // Temporary scene resources used to verify the Diligent geometry pass.
        std::unique_ptr<Mesh> m_testMesh;
        std::unique_ptr<Material> m_testMaterial;
        std::unique_ptr<Material> m_testMaterial2;
        std::vector<std::unique_ptr<ModelRenderer>> m_models;

    private:
        RenderSystem* m_render = nullptr;
    };
}

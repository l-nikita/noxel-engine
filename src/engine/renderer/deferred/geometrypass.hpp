#pragma once

#include <unordered_map>

#include "renderer/camera.hpp"
#include "renderer/material.hpp"
#include "renderer/modelrenderer.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/shader_cbuffers.hpp"

namespace render::deferred
{
    //-----------------------------------------------------------------------------

    class GBuffer;
    class DeferredRenderPipeline;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct GeometryPassContext
    {
        GBuffer* GBuffer;
        Camera* Camera;
        const std::vector<std::unique_ptr<ModelRenderer>>& ModelRenderers;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class GeometryPass final
    {
    public:
        GeometryPass( RenderSystem* render, DeferredRenderPipeline* pipeline );
        ~GeometryPass();

        void Setup();
        void SetupPSO();
        void SetupConstantBuffers();

        IShaderResourceBinding* GetGeometryBinding( const Material& material );

        void Render( const GeometryPassContext& ctx );

        void ApplyTranform( const glm::mat4& modelMatrix, const Camera* camera );
        void ApplyMaterialParameters( const Material& material );

    private:
        Diligent::RefCntAutoPtr<IPipelineState> m_geometryPSO;
        Diligent::RefCntAutoPtr<IBuffer> m_transformCB;
        Diligent::RefCntAutoPtr<IBuffer> m_tintCB;
        Diligent::RefCntAutoPtr<IBuffer> m_matFactorsCB;

        std::unordered_map<const Material*, Diligent::RefCntAutoPtr<IShaderResourceBinding>> m_geometryBindings;

    private:
        RenderSystem* m_render;
        DeferredRenderPipeline* m_pipeline;
    };
}

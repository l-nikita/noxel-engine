#pragma once

#include <unordered_map>

#include "assetsystem/shaderdata.hpp"
#include "renderer/camera.hpp"
#include "renderer/material.hpp"
#include "renderer/modelrenderer.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/deferred/gbuffer.hpp"
#include "renderer/deferred/passes/deferredpass.hpp"

namespace render::deferred::passes
{
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
    class GeometryPass final : public DeferredPass
    {
    public:
        explicit GeometryPass( const DeferredPassContext& ctx );
        ~GeometryPass();

        void Setup();
        void Render( const GeometryPassContext& ctx );

    private:
        void SetupPSO();
        void SetupConstantBuffers();

        void ApplyTransform( const glm::mat4& modelMat, const Camera* camera );
        void ApplyMaterialParameters( const Material& material );

        IShaderResourceBinding* GetGeometryBinding( const Material& material );

    private:
        // Geometry pass shader data
        std::unique_ptr<assets::ShaderData> m_shaderData;

        Diligent::RefCntAutoPtr<IPipelineState> m_PSO;
        Diligent::RefCntAutoPtr<IBuffer> m_transformCB;
        Diligent::RefCntAutoPtr<IBuffer> m_tintCB;
        Diligent::RefCntAutoPtr<IBuffer> m_matFactorsCB;

        std::unordered_map<const Material*, Diligent::RefCntAutoPtr<IShaderResourceBinding>> m_bindings;
    };
}

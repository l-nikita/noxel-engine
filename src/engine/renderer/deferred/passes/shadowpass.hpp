#pragma once

#include "assetsystem/shaderdata.hpp"
#include "renderer/lighting.hpp"
#include "renderer/modelrenderer.hpp"
#include "renderer/deferred/passes/deferredpass.hpp"

namespace render::deferred::passes
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct ShadowPassContext
    {
        lighting::Light* Light;
        const std::vector<ModelRenderer*>& ModelRenderers;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class ShadowPass final : public DeferredPass
    {
    public:
        explicit ShadowPass( const DeferredPassContext& ctx );

        void Setup();
        void Render( const ShadowPassContext& ctx );

    private:
        void SetupPSO();
        void SetupConstantBuffers();
        void SetupSRB();

        void ApplyTransform( const glm::mat4& modelMat, const lighting::Light* light );

    private:
        std::unique_ptr<assets::ShaderData> m_shaderData;

        Diligent::RefCntAutoPtr<IPipelineState> m_PSO;
        Diligent::RefCntAutoPtr<IShaderResourceBinding> m_SRB;

        Diligent::RefCntAutoPtr<IBuffer> m_transformCB;
    };
}

#pragma once

#include "assetsystem/shaderdata.hpp"
#include "renderer/camera.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/deferred/gbuffer.hpp"
#include "renderer/deferred/passes/deferredpass.hpp"

namespace render::deferred::passes
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct LightingPassContext
    {
        GBuffer* GBuffer;
        Camera* Camera;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class LightingPass final : public DeferredPass
    {
    public:
        explicit LightingPass( const DeferredPassContext& ctx );

        void Setup();
        void Render( const LightingPassContext& ctx );

    private:
        void SetupPSO();
        void SetupConstantBuffers();
        void SetupSRB();

    private:
        // Lighting pass shader data
        std::unique_ptr<assets::ShaderData> m_shaderData;

        Diligent::RefCntAutoPtr<IPipelineState> m_PSO;
        Diligent::RefCntAutoPtr<IShaderResourceBinding> m_SRB;
        Diligent::RefCntAutoPtr<IBuffer> m_lightingCB;
    };
}

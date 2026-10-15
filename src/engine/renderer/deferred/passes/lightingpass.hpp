#pragma once

#include "assetsystem/shaderdata.hpp"
#include "renderer/camera.hpp"
#include "renderer/lighting.hpp"
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
        deferred::GBuffer* GBuffer;
        render::Camera* Camera;
        ITextureView* SkyTextureView;
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

        void UpdateLights();
        void UpdateShadows();
        void UpdateView( const Camera* camera );

        int GetShadowSlot( const lighting::Light* lightA ) const;

        std::array<const lighting::Light*, lighting::MAX_SHADOWS_MAPS> m_shadowSlots = {};

    private:
        // Lighting pass shader data
        std::unique_ptr<assets::ShaderData> m_shaderData;

        Diligent::RefCntAutoPtr<IPipelineState> m_PSO;
        Diligent::RefCntAutoPtr<IShaderResourceBinding> m_SRB;
        Diligent::RefCntAutoPtr<IBuffer> m_lightingCB;
        Diligent::RefCntAutoPtr<IBuffer> m_viewCB;
    };
}

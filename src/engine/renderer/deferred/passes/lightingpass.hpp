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
        GBuffer* GBuffer;
        Camera* Camera;
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
        void UpdateView( const Camera* camera );

    private:
        // Lighting pass shader data
        std::unique_ptr<assets::ShaderData> m_shaderData;

        Diligent::RefCntAutoPtr<IPipelineState> m_PSO;
        Diligent::RefCntAutoPtr<IShaderResourceBinding> m_SRB;
        Diligent::RefCntAutoPtr<IBuffer> m_lightingCB;
        Diligent::RefCntAutoPtr<IBuffer> m_viewCB;

        Light* m_light1 = nullptr;
        Light* m_light2 = nullptr;
        Light* m_light4 = nullptr;
        Light* m_sunLight = nullptr;

        Light* m_flashlight = nullptr;
    };
}

#pragma once

#include "renderer/irenderpipeline.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    class Camera;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct RenderTarget
    {
        Diligent::RefCntAutoPtr<ITexture> Texture;
        ITextureView* RTV = nullptr;
        ITextureView* SRV = nullptr;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct GBuffer
    {
        RenderTarget Color;
        RenderTarget Normal;
        RenderTarget Position;
        RenderTarget Specular;

        Diligent::RefCntAutoPtr<ITexture> Depth;
        ITextureView* DSV = nullptr;

        void Release()
        {
            Color = {};
            Normal = {};
            Position = {};
            Specular = {};
            Depth.Release();
            DSV = nullptr;
        }
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class DeferredRenderPipeline final : public IRenderPipeline
    {
    public:
        SYSTEM_METADATA( RENDER_PIPELINE, "DeferredRenderPipeline", SysId::RENDER );

    public:
        DeferredRenderPipeline() = default;

        SysInit Initialize() override;
        void Shutdown() override;

        void Render() override;

        void DoGeometryPass() const;
        void DoShadowPass() const;
        void DoLightingPass() const;
        void DoSkyboxPass() const;

    private:
        void SetupGBuffer( std::uint32_t width, std::uint32_t height );
        void SetupGeometryPass();
        void SetupShadows();
        void SetupSkybox();

        void ClearBackBuffer();

    private:
        GBuffer m_GBuffer{};

        Diligent::RefCntAutoPtr<IPipelineState> m_geometryPSO;

        Camera* m_mainCamera = nullptr;
    };
}

#pragma once

#include "renderer/rendersystem.hpp"

namespace render::deferred
{
    //-----------------------------------------------------------------------------

    class DeferredRenderPipeline;

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
    class GBuffer final
    {
    public:
        GBuffer( RenderSystem* render, DeferredRenderPipeline* pipeline );
        ~GBuffer();

        void Setup( std::uint32_t width, std::uint32_t height );
        void Release();

        const RenderTarget& GetColor() const;
        const RenderTarget& GetNormal() const;
        const RenderTarget& GetPosition() const;
        const RenderTarget& GetSpecular() const;

        Diligent::RefCntAutoPtr<ITexture> GetDepth() const;
        ITextureView* GetDSV() const;

    private:
        RenderTarget m_color;
        RenderTarget m_normal;
        RenderTarget m_position;
        RenderTarget m_specular;

        Diligent::RefCntAutoPtr<ITexture> m_depth;
        ITextureView* m_DSV = nullptr;

    private:
        RenderSystem* m_render;
        DeferredRenderPipeline* m_pipeline;
    };
}

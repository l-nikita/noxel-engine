#include "renderer/deferred/gbuffer.hpp"
#include "dbg/debug.hpp"

namespace render::deferred
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    GBuffer::GBuffer( RenderSystem* render, DeferredRenderPipeline* pipeline )
        : m_render( render ), m_pipeline( pipeline )
    {
    }

    //-----------------------------------------------------------------------------
    GBuffer::~GBuffer()
    {
        Release();
    }

    void GBuffer::Setup( std::uint32_t width, std::uint32_t height )
    {
        Release();

        auto device = m_render->GetDevice();

        const auto createRenderTarget = [&]( RenderTarget& rt, const char* name, TEXTURE_FORMAT format )
        {
            TextureDesc desc;
            desc.Name = name;
            desc.Type = RESOURCE_DIM_TEX_2D;
            desc.Width = width;
            desc.Height = height;
            desc.Format = format;
            desc.MipLevels = 1;
            desc.BindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
            desc.Usage = USAGE_DEFAULT;
            desc.ClearValue.SetColor( desc.Format, CLEAR_COLOR );

            device->CreateTexture( desc, nullptr, &rt.Texture );

            if ( !rt.Texture )
                FATAL_ERROR( std::string( "Failed to create " ) + name, cmd::RENDERING );

            rt.RTV = rt.Texture->GetDefaultView( TEXTURE_VIEW_RENDER_TARGET );
            rt.SRV = rt.Texture->GetDefaultView( TEXTURE_VIEW_SHADER_RESOURCE );
        };

        createRenderTarget( m_color, "GBuffer Color", TEX_FORMAT_RGBA8_UNORM );
        createRenderTarget( m_normal, "GBuffer Normal", TEX_FORMAT_RGBA8_UNORM );
        createRenderTarget( m_position, "GBuffer Position", TEX_FORMAT_RGBA16_FLOAT );
        createRenderTarget( m_specular, "GBuffer Specular", TEX_FORMAT_RGBA8_UNORM );

        TextureDesc depthDesc;
        depthDesc.Name = "GBuffer Depth";
        depthDesc.Type = RESOURCE_DIM_TEX_2D;
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.Format = TEX_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.MipLevels = 1;
        depthDesc.BindFlags = BIND_DEPTH_STENCIL;
        depthDesc.Usage = USAGE_DEFAULT;

        depthDesc.ClearValue.SetDepthStencil( depthDesc.Format, 1.0f, 0 );
        device->CreateTexture( depthDesc, nullptr, &m_depth );

        if ( !m_depth )
            FATAL_ERROR( "Failed to create GBuffer Depth", cmd::RENDERING );

        m_DSV = m_depth->GetDefaultView( TEXTURE_VIEW_DEPTH_STENCIL );
    }

    void GBuffer::Release()
    {
        m_color = {};
        m_normal = {};
        m_position = {};
        m_specular = {};
        m_depth.Release();
        m_DSV = nullptr;
    }

    const RenderTarget& GBuffer::GetColor() const
    {
        return m_color;
    }

    const RenderTarget& GBuffer::GetNormal() const
    {
        return m_normal;
    }

    const RenderTarget& GBuffer::GetPosition() const
    {
        return m_position;
    }

    const RenderTarget& GBuffer::GetSpecular() const
    {
        return m_specular;
    }

    RefCntAutoPtr<ITexture> GBuffer::GetDepth() const
    {
        return m_depth;
    }

    ITextureView* GBuffer::GetDSV() const
    {
        return m_DSV;
    }
}

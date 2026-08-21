#pragma once

#include "dbg/debug.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/lighting/lightcommon.hpp"

namespace render::lighting
{
    class ShadowMap final
    {
    public:
        explicit ShadowMap( IRenderDevice* device, const std::uint32_t size = SHADOW_MAP_SIZE,
                            bool createTexture = true )
            : m_size( size )
        {
            if ( !createTexture )
                return;

            CreateTexture( device );
        }

        const std::uint32_t& GetSize() const { return m_size; }

        ITextureView* GetDSV() const { return m_DSV; }
        ITextureView* GetSRV() const { return m_SRV; }

    private:
        void CreateTexture( IRenderDevice* device )
        {
            using namespace Diligent;

            TextureDesc depthDesc;
            depthDesc.Name = "ShadowMap";
            depthDesc.Type = RESOURCE_DIM_TEX_2D;
            depthDesc.Width = m_size;
            depthDesc.Height = m_size;
            depthDesc.Format = TEX_FORMAT_D24_UNORM_S8_UINT;
            depthDesc.MipLevels = 1;
            depthDesc.BindFlags = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;
            depthDesc.Usage = USAGE_DEFAULT;

            depthDesc.ClearValue.SetDepthStencil( depthDesc.Format, 1.0f, 0 );
            device->CreateTexture( depthDesc, nullptr, &m_depth );

            if ( !m_depth )
                FATAL_ERROR( "Failed to create ShadowMap", cmd::RENDERING );

            m_DSV = m_depth->GetDefaultView( TEXTURE_VIEW_DEPTH_STENCIL );
            m_SRV = m_depth->GetDefaultView( TEXTURE_VIEW_SHADER_RESOURCE );
        }

    private:
        std::uint32_t m_size = SHADOW_MAP_SIZE;

        Diligent::RefCntAutoPtr<ITexture> m_depth;
        ITextureView* m_DSV = nullptr;
        ITextureView* m_SRV = nullptr;
    };
}

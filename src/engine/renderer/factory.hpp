#pragma once

#include "assetsystem/asset.hpp"
#include "assetsystem/shaderdata.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    struct Shader
    {
        Diligent::RefCntAutoPtr<IShader> VertexShader;
        Diligent::RefCntAutoPtr<IShader> PixelShader;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Factory final
    {
    public:
        explicit Factory( RenderSystem* render );

        Shader CreateShader( const assets::Id& id );
        void CreatePipelineState( Diligent::GraphicsPipelineStateCreateInfo& info,
                                  const assets::ShaderData& data, IPipelineState** state );

        Diligent::RefCntAutoPtr<ITexture> CreateTexture( const assets::Id& id );
        Diligent::RefCntAutoPtr<ITexture> CreateSolidTexture( const std::array<std::uint8_t, 4>& color,
                                                              const std::string& name );

    private:
        Diligent::RefCntAutoPtr<IShaderSourceInputStreamFactory> m_shaderSourceFactory;

    private:
        RenderSystem* m_render;
    };
}

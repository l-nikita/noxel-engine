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

    struct TextureCubeData
    {
        std::array<assets::Id, 6> Faces;
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
        Diligent::RefCntAutoPtr<ITexture> CreateTextureCube( const TextureCubeData& cubeData );
        Diligent::RefCntAutoPtr<ITexture> CreateTextureCube( const assets::Id& matId );

        Diligent::RefCntAutoPtr<ITexture> CreateSolidTexture( const std::array<std::uint8_t, 4>& color,
                                                              const std::string& name );

        Diligent::RefCntAutoPtr<ITexture> CreateSolidTextureCube( const std::array<std::uint8_t, 4>& color,
                                                                  const std::string& name );

        static Diligent::FILTER_TYPE ParseFilter( const std::string& name, bool comparison );
        static Diligent::TEXTURE_ADDRESS_MODE ParseAddressMode( const std::string& name );
        static Diligent::COMPARISON_FUNCTION ParseComparisonFunc( const std::string& name );

    private:
        Diligent::RefCntAutoPtr<IShaderSourceInputStreamFactory> m_shaderSourceFactory;

    private:
        RenderSystem* m_render;
    };
}

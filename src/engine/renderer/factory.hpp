#pragma once

#include "assetsystem/asset.hpp"
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
    class Factory
    {
    public:
        static Shader CreateShader( const assets::Id& id );
        static Diligent::RefCntAutoPtr<ITexture> CreateTexture( const assets::Id& id );
        static Diligent::RefCntAutoPtr<ITexture> CreateSolidTexture( const std::array<std::uint8_t, 4>& color,
                                                                     const std::string& name );
    };
}

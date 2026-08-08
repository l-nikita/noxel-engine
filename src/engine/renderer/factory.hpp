#pragma once

#include "assetsystem/asset.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    using ShaderPsVs = std::tuple<Diligent::RefCntAutoPtr<IShader>, Diligent::RefCntAutoPtr<IShader>>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Factory
    {
    public:
        static ShaderPsVs CreateShader( const assets::Id& id );
        static Diligent::RefCntAutoPtr<ITexture> CreateTexture( const assets::Id& id );
    };
}

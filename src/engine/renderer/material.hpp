#pragma once

#include "assetsystem/materialdata.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    struct MaterialData;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Material
    {
    public:
        Material( IRenderDevice* device, const assets::MaterialData& data );

    private:
        std::unordered_map<std::string, Diligent::RefCntAutoPtr<ITexture>> m_textures;
    };
}

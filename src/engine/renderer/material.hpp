#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "assetsystem/materialdata.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    struct MaterialData;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Material final
    {
    public:
        explicit Material( const assets::MaterialData& data );

        ITexture* GetTexture( const std::string& name ) const;
        const assets::Id& GetShaderId() const;

        const assets::MaterialParameters& GetParameters() const;

    private:
        assets::Id m_shaderId;

        assets::MaterialParameters m_texturesParams;
        std::unordered_map<std::string, Diligent::RefCntAutoPtr<ITexture>> m_textures;
    };
}

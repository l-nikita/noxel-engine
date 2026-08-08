#include <ranges>

#include "renderer/material.hpp"
#include "assetsystem/iassetsystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    Material::Material( IRenderDevice* device, const assets::MaterialData& data )
    {
        for ( const auto& paramVal: data.Parameters | std::views::values )
        {
            if ( const auto* id = std::get_if<assets::Id>( &paramVal ) )
            {
                const auto ass = g_Assets->GetAsset( *id );
                if ( ass->Type == assets::Type::TEXTURE )
                {
                    // m_textures[ paramName ] = factory->CreateTexture( ass->Id );
                }
            }
        }
    }
}

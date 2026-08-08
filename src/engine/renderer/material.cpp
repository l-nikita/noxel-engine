
#include "renderer/material.hpp"
#include "renderer/factory.hpp"
#include "assetsystem/iassetsystem.hpp"

namespace render
{
    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    Material::Material( const assets::MaterialData& data )
        : m_shaderId( data.Shader ), m_params( data.Parameters )
    {
        for ( const auto& [ paramName, paramVal ]: data.Parameters )
        {
            if ( const auto* id = std::get_if<assets::Id>( &paramVal ) )
            {
                const auto* asset = g_Assets->GetAsset( *id );
                if ( asset && asset->Type == assets::Type::TEXTURE )
                    m_textures[ paramName ] = Factory::CreateTexture( *id );
            }
        }
    }

    ITexture* Material::GetTexture( const std::string& name ) const
    {
        const auto it = m_textures.find( name );
        return it != m_textures.end() ? it->second.RawPtr() : nullptr;
    }

    const assets::Id& Material::GetShaderId() const
    {
        return m_shaderId;
    }

    const assets::MaterialParameters& Material::GetParameters() const
    {
        return m_params;
    }
}

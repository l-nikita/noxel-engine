
#include "renderer/material.hpp"

#include <ranges>

#include "renderer/factory.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/shaderdata.hpp"

namespace render
{
    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    Material::Material( const assets::MaterialData& data )
        : m_shaderId( data.Shader ), m_texturesParams( data.TexturesParameters )
    {
        auto shaderData = g_Assets->LoadData<assets::ShaderData>( m_shaderId );
        assert( shaderData );

        const auto createError = [this]( const std::string& paramName )
        {
            m_textures[ paramName ] = Factory::CreateTexture( assets::ERROR_TEXTURE_ID );
        };

        for ( const auto& [ paramName, shaderParam ]: shaderData->Textures )
        {
            const auto it = data.TexturesParameters.find( paramName );

            if ( it == data.TexturesParameters.end() )
            {
                createError( paramName );
                continue;
            }

            const auto& matParam = it->second;

            if ( const auto* id = std::get_if<assets::Id>( &matParam ) )
            {
                const auto asset = g_Assets->GetAsset( *id );

                if ( !asset || asset->Type != assets::Type::TEXTURE )
                {
                    createError( paramName );
                    continue;
                }

                m_textures[ paramName ] = Factory::CreateTexture( *id );
                continue;
            }

            m_textures.erase( paramName );
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
        return m_texturesParams;
    }
}

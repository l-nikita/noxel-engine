#pragma once

#include <nlohmann/json.hpp>

#include "assetsystem/asset.hpp"
#include "assetsystem/iassetloader.hpp"
#include "assetsystem/shaderdata.hpp"

namespace assets::loaders
{
    //-----------------------------------------------------------------------------

    using ShaderDataLoad = std::expected<ShaderData*, std::string>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class ShaderLoader : public IAssetLoader<ShaderData>
    {
    public:
        ~ShaderLoader() override = default;

        ShaderDataLoad Load( fs::IFileSystem* fileSystem, const Asset* asset ) override
        {
            if ( !fileSystem ) return std::unexpected( "Filesystem is invalid" );
            if ( !asset ) return std::unexpected( "Asset is invalid" );

            auto jsonStr = fileSystem->ReadText( asset->Path );
            if ( jsonStr.empty() )
                return std::unexpected( "Shader meta is invalid: " + asset->Path.string() );

            try
            {
                nlohmann::json json = nlohmann::json::parse( jsonStr );

                auto data = new ShaderData();
                data->Name = json.value( "name", asset->Name );

                if ( json.contains( "pixel_shader" ) )
                    data->PixelShaderPath = json[ "pixel_shader" ].value( "file", "" );

                if ( json.contains( "vertex_shader" ) )
                    data->VertexShaderPath = json[ "vertex_shader" ].value( "file", "" );

                const auto shaderDir = asset->Path.parent_path();
                if ( !data->PixelShaderPath.empty() )
                    data->PixelShaderPath = ( shaderDir / data->PixelShaderPath ).lexically_normal();
                if ( !data->VertexShaderPath.empty() )
                    data->VertexShaderPath = ( shaderDir / data->VertexShaderPath ).lexically_normal();

                if ( data->PixelShaderPath.empty() || data->VertexShaderPath.empty() )
                {
                    delete data;
                    return std::unexpected( "Shader paths are missing in: " + asset->Path.string() );
                }

                // "parameters": "textures"
                for ( auto [ paramName, paramVal ]: json[ "parameters" ][ "textures" ].items() )
                {
                    auto varBind = paramVal.value( "var_bind", "" );
                    auto varType = paramVal.value( "var_type", "" );
                    auto defTex = paramVal.value( "def_texture", "" );

                    data->Textures[ paramName ] = {
                        varBind,
                        varType,
                        defTex
                    };
                }

                // "parameters": "samplers"
                for ( auto [ paramName, paramVal ]: json[ "parameters" ][ "samplers" ].items() )
                {
                    auto minFilter = paramVal.value( "min_filter", "" );
                    auto magFilter = paramVal.value( "mag_filter", "" );
                    auto mipFilter = paramVal.value( "mip_filter", "" );
                    auto addressU = paramVal.value( "address_u", "" );
                    auto addressV = paramVal.value( "address_v", "" );
                    auto addressW = paramVal.value( "address_w", "" );

                    data->Samplers[ paramName ] = {
                        minFilter,
                        magFilter,
                        mipFilter,
                        addressU,
                        addressV,
                        addressW,
                    };
                }

                return data;
            } catch ( const nlohmann::json::exception& e )
            {
                return std::unexpected(
                    "JSON parse error: " + std::string( e.what() ) + "\nFile: " + asset->Path.string() );
            }
        }
    };
}

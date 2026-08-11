#pragma once

#include <nlohmann/json.hpp>

#include "assetsystem/asset.hpp"
#include "assetsystem/iassetloader.hpp"
#include "assetsystem/materialdata.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
namespace assets::loaders
{
    //-----------------------------------------------------------------------------

    using MaterialDataLoad = std::expected<MaterialData*, std::string>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class MaterialLoader : public IAssetLoader<MaterialData>
    {
    public:
        ~MaterialLoader() override = default;

        MaterialDataLoad Load( fs::IFileSystem* fileSystem, const Asset* asset ) override
        {
            if ( !fileSystem ) return std::unexpected( "Filesystem is invalid" );
            if ( !asset ) return std::unexpected( "Asset is invalid" );

            auto jsonStr = fileSystem->ReadText( asset->Path );
            if ( jsonStr.empty() )
                return std::unexpected( "Material meta is invalid: " + asset->Path.string() );

            try
            {
                nlohmann::json json = nlohmann::json::parse( jsonStr );

                auto data = new MaterialData();
                data->AssetId = asset->Id;

                auto shaderAss = g_Assets->FindAsset( json[ "shader" ].get<std::string>(), Type::SHADER );
                if ( !shaderAss )
                {
                    delete data;
                    return std::unexpected( "Shader asset is invalid: " + asset->Path.string() );
                }

                data->Shader = shaderAss.value();

                if ( json.contains( "is_cubemap" ) )
                    data->IsCubemap = json[ "is_cubemap" ].get<bool>();
                else
                    data->IsCubemap = false;

                for ( const auto& [ paramName, param ]: json[ "parameters" ][ "textures" ].items() )
                {
                    if ( param.is_number_integer() )
                        data->TexturesParameters[ paramName ] = param.get<int>();
                    else if ( param.is_number_float() )
                        data->TexturesParameters[ paramName ] = param.get<float>();
                    else if ( param.is_string() )
                    {
                        const auto value = param.get<std::string>();

                        if ( const auto texture = g_Assets->FindAsset( value, Type::TEXTURE ) )
                            data->TexturesParameters[ paramName ] = texture.value();
                        else
                        {
                            data->TexturesParameters[ paramName ] = g_Assets->GetErrorAsset( Type::TEXTURE );
                            CMD_ERROR( "Texture not found: " + value + "\nMaterial: " + asset->Path.string(),
                                       cmd::ASSETSYSTEM );
                        }
                    }
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

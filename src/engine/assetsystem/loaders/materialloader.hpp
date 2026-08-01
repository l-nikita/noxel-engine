#pragma once

#include <nlohmann/json.hpp>

#include "assetsystem/asset.hpp"
#include "assetsystem/assetsystem.hpp"
#include "assetsystem/iassetloader.hpp"
#include "assetsystem/materialdata.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using MaterialDataLoad = std::expected<MaterialData*, std::string>;

class MaterialLoader : public IAssetLoader<MaterialData>
{
public:
    ~MaterialLoader() override = default;

    MaterialDataLoad Load( IFileSystem* fileSystem, const Asset* asset ) override
    {
        if ( !fileSystem ) return std::unexpected( "Filesystem is invalid" );
        if ( !asset ) return std::unexpected( "Asset is invalid" );

        auto jsonStr = fileSystem->ReadText( asset->Path );
        if ( !jsonStr.data() || jsonStr.empty() )
            return std::unexpected( "Material meta is invalid: " + asset->Path.string() );

        try
        {
            nlohmann::json json = nlohmann::json::parse( jsonStr );

            auto data = new MaterialData();
            data->Id = asset->Id;

            auto shaderAss = g_Assets->GetAsset( json[ "shader" ].get<std::string>(), AssetType::SHADER );
            if ( !shaderAss )
                return std::unexpected( "Shader asset is invalid: " + asset->Path.string() );

            data->Shader = shaderAss.value();

            if ( json.contains( "iscubemap" ) )
                data->IsCubemap = json[ "iscubemap" ].get<bool>();
            else
                data->IsCubemap = false;

            for ( const auto& [ paramName, param ]: json[ "parameters" ].items() )
            {
                if ( param.is_boolean() )
                    data->Parameters[ paramName ] = param.get<bool>();
                else if ( param.is_number_integer() )
                    data->Parameters[ paramName ] = param.get<int>();
                else if ( param.is_number_float() )
                    data->Parameters[ paramName ] = param.get<float>();
                else if ( param.is_string() )
                {
                    auto paramStr = param.get<std::string>();
                    if ( auto texAsset = g_Assets->GetAsset( paramStr, AssetType::TEXTURE ) )
                        data->Parameters[ paramName ] = texAsset.value();
                    else
                        data->Parameters[ paramName ] = paramStr;
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

#pragma once

#include <nlohmann/json.hpp>

#include "assetsystem/materialdata.hpp"
#include "assetsystem/asset.hpp"
#include "assetsystem/iassetloader.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
inline void from_json( const nlohmann::json& json, MaterialData& data )
{
    data.Shader = json.value( "shader", "error" );
    data.IsCubemap = json.value( "iscubemap", false );

    if ( json.contains( "parameters" ) )
        data.Parameters = json[ "parameters" ];
}

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

            auto data = new MaterialData( json.get<MaterialData>() );
            return data;
        } catch ( const nlohmann::json::exception& e )
        {
            return std::unexpected(
                "JSON parse error: " + std::string( e.what() ) + "; File: " + asset->Path.string() );
        }
    }
};

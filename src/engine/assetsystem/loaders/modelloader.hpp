#pragma once

#include <nlohmann/json.hpp>

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include "assetsystem/asset.hpp"
#include "assetsystem/iassetloader.hpp"
#include "assetsystem/modeldata.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using ModelDataLoad = std::expected<ModelData*, std::string>;

class ModelLoader : public IAssetLoader<ModelData>
{
public:
    ~ModelLoader() override = default;

    ModelDataLoad Load( IFileSystem* fileSystem, const Asset* asset ) override
    {
        if ( !fileSystem ) return std::unexpected( "Filesystem is invalid" );
        if ( !asset ) return std::unexpected( "Asset is invalid" );

        auto jsonStr = fileSystem->ReadText( asset->Path );
        if ( !jsonStr.data() || jsonStr.empty() )
            return std::unexpected( "Model meta is invalid: " + asset->Path.string() );

        try
        {
            nlohmann::json json = nlohmann::json::parse( jsonStr );

            auto data = new ModelData( json.get<ModelData>() );
            return data;
        } catch ( const nlohmann::json::exception& e )
        {
            return std::unexpected( "JSON parse error: " + std::string( e.what() ) );
        }
    }
};

#pragma once

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

    ModelDataLoad Load( IFileSystem* fileSystem, const Asset* asset ) override;
};

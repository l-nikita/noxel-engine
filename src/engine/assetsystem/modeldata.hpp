#pragma once

#include "assetsystem/asset.hpp"

namespace assets
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct ModelData : IAssetData
    {
        fs::Path MeshPath;
    };
}

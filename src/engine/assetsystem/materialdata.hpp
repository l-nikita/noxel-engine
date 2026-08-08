#pragma once

#include <unordered_map>
#include <variant>

#include "assetsystem/asset.hpp"

namespace assets
{
    //-----------------------------------------------------------------------------

    using MaterialParam = std::variant<
        int,
        float,
        bool,
        std::string,
        Id>;

    using MaterialParameters = std::unordered_map<std::string, MaterialParam>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct MaterialData : IAssetData
    {
        Id AssetId;
        Id Shader;
        bool IsCubemap = false;
        MaterialParameters Parameters;
    };
}

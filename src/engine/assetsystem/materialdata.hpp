#pragma once

#include <variant>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "assetsystem/asset.hpp"

//-----------------------------------------------------------------------------

using MaterialParam = std::variant<
    int,
    float,
    bool,
    std::string,
    AssetId>;

using MaterialParameters = std::unordered_map<std::string, MaterialParam>;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct MaterialData
{
    AssetId Id;
    AssetId Shader;
    bool IsCubemap = false;
    MaterialParameters Parameters;
};

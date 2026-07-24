#pragma once

#include "assettype.hpp"
#include "filesystem/ifilesystem.hpp"

struct Asset
{
    std::string Name;
    FPath Path;
    AssetType Type;
};

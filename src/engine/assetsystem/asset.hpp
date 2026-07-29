#pragma once

#include <boost/uuid/uuid.hpp>

#include "assettype.hpp"
#include "filesystem/ifilesystem.hpp"

using AssetId = boost::uuids::uuid;

struct Asset
{
    AssetId UUID;
    std::string Name;
    FPath Path;
    AssetType Type;
};

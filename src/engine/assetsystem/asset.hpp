#pragma once

#include <boost/uuid/uuid.hpp>

#include "assettype.hpp"
#include "filesystem/ifilesystem.hpp"

namespace assets
{
    using Id = boost::uuids::uuid;

    struct Asset
    {
        Id Id;
        std::string Name;
        fs::Path Path;
        Type Type;
    };

    struct IAssetData
    {
        virtual ~IAssetData() = default;
    };
}

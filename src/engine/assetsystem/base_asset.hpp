#pragma once

#include <filesystem>
#include <string>
#include "asset_type.hpp"

class BaseAsset
{
public:
    virtual ~BaseAsset() = default;

    virtual AssetType GetType() const
    {
        return Type;
    }

    static constexpr auto Type = AssetType::INVALID;

    virtual std::string GetName() const
    {
        return m_name;
    }

    virtual std::filesystem::path GetAssetPath() const
    {
        return m_assetPath;
    }

protected:
    std::string m_name;
    std::filesystem::path m_assetPath;
};

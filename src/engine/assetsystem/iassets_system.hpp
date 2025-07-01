#pragma once

#include <filesystem>
#include <unordered_map>

#include "base_asset.hpp"

typedef std::shared_ptr<BaseAsset> AssetPtr;
typedef std::unordered_map<std::string, AssetPtr> AssetsMap;

class IAssetsSystem
{
public:
    virtual ~IAssetsSystem() = default;

    virtual bool Init() = 0;

    virtual std::filesystem::path GetRootPath() = 0;
    virtual std::filesystem::path GetBinPath() = 0;
    virtual std::filesystem::path GetAssetsPath() = 0;

    virtual std::filesystem::path GetMaterialsPath() = 0;

    virtual void RegisterAsset(AssetPtr asset) = 0;

    virtual AssetsMap GetAssets(AssetType type) = 0;
    virtual AssetPtr HasAsset(AssetType type, const std::string& name) = 0;
};

extern IAssetsSystem* g_Assets;
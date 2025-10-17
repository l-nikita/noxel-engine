#pragma once

#include <filesystem>
#include <unordered_map>

#include "base_asset.hpp"

typedef std::unordered_map<std::string, BaseAsset*> AssetsMap;

class IAssetSystem
{
public:
    virtual ~IAssetSystem() = default;

    virtual void Init() = 0;

    virtual std::filesystem::path GetRootPath() = 0;
    virtual std::filesystem::path GetBinPath() = 0;
    virtual std::filesystem::path GetAssetsPath() = 0;

    virtual std::filesystem::path GetMaterialsPath() = 0;

    virtual void RegisterAsset(BaseAsset* asset) = 0;

    virtual const AssetsMap& GetAssets(AssetType type) = 0;
    virtual BaseAsset* GetAsset(AssetType type, const std::string& name) = 0;
};

extern IAssetSystem* g_Assets;
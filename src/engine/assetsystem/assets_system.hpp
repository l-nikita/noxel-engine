#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <memory>

#include "singleton.hpp"
#include "asset_type.hpp"
#include "base_asset.hpp"

typedef std::shared_ptr<BaseAsset> AssetPtr;
typedef std::unordered_map<std::string, AssetPtr> AssetsMap;

class AssetsSystem : public Singleton<AssetsSystem>
{
private:
	std::unordered_map<AssetType, AssetsMap> m_assets;
    std::filesystem::path m_rootPath;
    std::filesystem::path m_binPath;
    std::filesystem::path m_assetsPath;

public:
	bool Init();

    static std::filesystem::path GetRootPath() { return GetInstance().m_rootPath; }
    static std::filesystem::path GetBinPath() { return GetInstance().m_binPath; }
    static std::filesystem::path GetAssetsPath() { return GetInstance().m_assetsPath; }

    static std::filesystem::path GetMaterialsPath() { return GetInstance().GetAssetsPath() / "materials"; }

	void RegisterAsset(AssetPtr asset);

    template <typename T>
    AssetsMap GetAssets()
    {
        AssetsMap result;

        auto it = m_assets.find(T::Type);
        if (it == m_assets.end())
            return result;

        return it->second;
    }

    AssetPtr HasAsset(AssetType type, const std::string& name) const
    {
        auto typeIt = m_assets.find(type);
        if (typeIt == m_assets.end())
            return nullptr;

        const auto& assetsMap = typeIt->second;

        auto assetIt = assetsMap.find(name);
        if (assetIt != assetsMap.end())
            return assetIt->second;

        return nullptr;
    }
};
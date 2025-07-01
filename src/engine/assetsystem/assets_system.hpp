#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>

#include "common/singleton.hpp"
#include "iassets_system.hpp"
#include "asset_type.hpp"
#include "base_asset.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class AssetsSystem : public IAssetsSystem
{
private:
	std::unordered_map<AssetType, AssetsMap> m_assets;
    std::filesystem::path m_rootPath;
    std::filesystem::path m_binPath;
    std::filesystem::path m_assetsPath;

public:
	bool Init() override;

    std::filesystem::path GetRootPath() override { return m_rootPath; }
    std::filesystem::path GetBinPath() override { return m_binPath; }
    std::filesystem::path GetAssetsPath() override { return m_assetsPath; }

    std::filesystem::path GetMaterialsPath() override { return GetAssetsPath() / "materials"; }

	void RegisterAsset(AssetPtr asset);

    AssetsMap GetAssets(AssetType type) override
    {
        auto it = m_assets.find(type);
        if (it == m_assets.end())
            return {};

        return it->second;
    }

    AssetPtr HasAsset(AssetType type, const std::string& name) override
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
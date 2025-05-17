#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>

#include "singleton.hpp"
#include "asset_type.hpp"
#include "base_asset.hpp"

typedef std::unordered_map<std::string, std::shared_ptr<BaseAsset>> AssetsMap;

class AssetsSystem : public Singleton<AssetsSystem>
{
private:
	std::unordered_map<AssetType, AssetsMap> m_assets;

public:
	bool Init();
	void RegisterAsset(std::shared_ptr<BaseAsset> asset);

    template <typename T>
    AssetsMap GetAssets()
    {
        AssetsMap result;

        auto it = m_assets.find(T::Type);
        if (it == m_assets.end())
            return result;

        return it->second;
    }
};
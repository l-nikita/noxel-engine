#pragma once

#include <unordered_map>
#include "assettype.hpp"
#include "baseasset.hpp"
#include "iassetsystem.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class AssetSystem : public IAssetSystem
{
    std::unordered_map<AssetType, AssetsMap> m_assets;
    std::filesystem::path m_rootPath;
    std::filesystem::path m_binPath;
    std::filesystem::path m_assetsPath;

public:
    SysInit Initialize() override;
    void Shutdown() override;

    std::filesystem::path GetRootPath() override
    {
        return m_rootPath;
    }

    std::filesystem::path GetBinPath() override
    {
        return m_binPath;
    }

    std::filesystem::path GetAssetsPath() override
    {
        return m_assetsPath;
    }

    std::filesystem::path GetMaterialsPath() override
    {
        return GetAssetsPath() / "materials";
    }

    void RegisterAsset( BaseAsset* asset ) override;

    const AssetsMap& GetAssets( const AssetType type ) override
    {
        const auto it = m_assets.find( type );
        if ( it == m_assets.end() )
            return m_assets[ type ];

        return it->second;
    }

    BaseAsset* GetAsset( const AssetType type, const std::string& name ) override
    {
        const auto typeIt = m_assets.find( type );
        if ( typeIt == m_assets.end() )
            return nullptr;

        const auto& assetsMap = typeIt->second;

        if ( const auto assetIt = assetsMap.find( name ); assetIt != assetsMap.end() )
            return assetIt->second;

        return nullptr;
    }
};

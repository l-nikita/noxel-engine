#pragma once

#include "iassetsystem.hpp"
#include "loaders/materialloader.hpp"
#include "loaders/shaderloader.hpp"
#include "loaders/textureloader.hpp"

namespace assets
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class AssetSystem final : public IAssetSystem
    {
    public:
        SYSTEM_METADATA( ASSET_SYSTEM, "AssetSystem", SysId::FILESYSTEM );

    public:
        SysInit Initialize() override;
        void Shutdown() override;

        const Assets& GetAssets() const override;
        std::optional<Id> FindAsset( const fs::Path& path, Type type ) const override;
        const Asset* GetAsset( const Id& assetId ) const override;
        Id GetErrorAsset( Type type ) const override;

        void RegisterAsset( const fs::Path& path, Type type ) override;

        bool IsSupportedImage( const fs::Path& file ) const override;

    protected:
        IAssetData* LoadDataImpl( const Asset* asset ) override;

    private:
        void SetupErrorAssets();
        void SearchAssets();
        void DetectAndRegisterAsset( const fs::Path& file );

    private:
        Assets m_assets = {};

        loaders::MaterialLoader m_materialLoader;
        loaders::ShaderLoader m_shaderLoader;
        loaders::TextureLoader m_textureLoader;
    };
}

#pragma once

#include "iassetsystem.hpp"
#include "loaders/materialloader.hpp"
#include "loaders/shaderloader.hpp"
#include "loaders/textureloader.hpp"

namespace assets
{
    //-----------------------------------------------------------------------------

    constexpr auto META_EXTENSION = ".meta";

    static const std::string SUPPORTED_IMAGE_FORMATS[ ] = {
        ".jpg", ".png"
    };

    static const std::string SUPPORTED_MESH_FORMATS[ ] = {
        ".glb", ".gltf"
    };

    constexpr auto MATERIAL_EXTENSION = ".nmt";
    constexpr auto SHADER_EXTENSION = ".nshader";
    constexpr auto MODEL_EXTENSION = ".nmdl";

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
        std::optional<Id> GetAsset( const fs::Path& path, Type type ) const override;
        const Asset* GetAsset( const Id& assetId ) const override;

        void RegisterAsset( const fs::Path& path, Type type ) override;

    protected:
        IAssetData* LoadDataImpl( const Asset* asset ) override;

    private:
        void SearchAssets();
        void DetectAndRegisterAsset( const fs::Path& file );

    private:
        Assets m_assets = {};

        loaders::MaterialLoader m_materialLoader;
        loaders::ShaderLoader m_shaderLoader;
        loaders::TextureLoader m_textureLoader;
    };
}

#pragma once

#include "iassetsystem.hpp"
#include "loaders/materialloader.hpp"
#include "loaders/shaderloader.hpp"
#include "loaders/textureloader.hpp"

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
    std::optional<AssetId> GetAsset( const FPath& path, AssetType type ) const override;
    const Asset* GetAsset( const AssetId& assetId ) const override;

    void RegisterAsset( const FPath& path, AssetType type ) override;

protected:
    void* LoadAssetImpl( const Asset* asset ) override;

private:
    void SearchAssets();
    void DetectAndRegisterAsset( const FPath& file );

private:
    Assets m_assets = {};

    MaterialLoader m_materialLoader;
    ShaderLoader m_shaderLoader;
    TextureLoader m_textureLoader;
};

#include <algorithm>
#include <cctype>
#include <filesystem>

#include "dbg/debug.hpp"
#include "assetsystem.hpp"
#include "filesystem/ifilesystem.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

SysInit AssetSystem::Initialize()
{
    SearchAssets();

    CMD::RegisterCommand( "as_print_assets", "Prints assets", [this]
    {
        for ( const auto& asset: m_assets )
            CMD_MSG( asset.Path.string(), CMD::ASSETSYSTEM );
    } );

    return {};
}

void AssetSystem::Shutdown()
{
    m_assets.clear();
}

const Assets& AssetSystem::GetAssets() const
{
    return m_assets;
}

const Asset* AssetSystem::GetAsset( const FPath& path, AssetType type ) const
{
    if ( path.empty() )
        return nullptr;

    const auto query = path.lexically_normal();
    const auto absoluteQuery = query.is_absolute()
                                   ? query
                                   : ( g_FileSystem->GetRootPath() / query ).lexically_normal();
    const auto queryName = query.filename().string();
    const auto queryStem = query.stem().string();

    for ( const auto& asset: m_assets )
    {
        if ( type == asset.Type && asset.Path == absoluteQuery )
            return &asset;
    }

    if ( query.has_parent_path() )
    {
        const auto queryPath = query.generic_string();
        for ( const auto& asset: m_assets )
            if ( type == asset.Type && asset.Path.generic_string().ends_with( queryPath ) )
                return &asset;
    } else
    {
        for ( const auto& asset: m_assets )
            if ( type == asset.Type && ( asset.Name == queryName || asset.Name == queryStem ) )
                return &asset;
    }

    return nullptr;
}

void AssetSystem::RegisterAsset( const FPath& path, AssetType type )
{
    const auto absolutePath = path.is_absolute()
                                  ? path.lexically_normal()
                                  : ( g_FileSystem->GetRootPath() / path ).lexically_normal();

    if ( !g_FileSystem->Exists( absolutePath ) )
    {
        CMD_ERROR( "File not found: " + absolutePath.string(), CMD::ASSETSYSTEM );
        return;
    }

    for ( const auto& asset: m_assets )
    {
        if ( asset.Path == absolutePath && asset.Type == type )
        {
            CMD_ERROR( "Asset with the same path and type is already registered", CMD::ASSETSYSTEM );
            return;
        }
    }

    m_assets.push_back( Asset{absolutePath.stem().string(), absolutePath, type} );
}

void* AssetSystem::LoadAssetImpl( const Asset* asset )
{
    if ( !asset )
    {
        CMD_ERROR( "Cannot load a null asset", CMD::ASSETSYSTEM );
        return nullptr;
    }

    std::expected<void*, std::string> result = std::unexpected( "Unsupported asset type" );

    switch ( asset->Type )
    {
        case AssetType::MATERIAL:
        {
            if ( auto loaded = m_materialLoader.Load( g_FileSystem, asset ) )
                result = loaded.value();
            else
                result = std::unexpected( loaded.error() );

            break;
        }
        case AssetType::SHADER:
        {
            if ( auto loaded = m_shaderLoader.Load( g_FileSystem, asset ) )
                result = loaded.value();
            else
                result = std::unexpected( loaded.error() );

            break;
        }
        case AssetType::TEXTURE:
        {
            if ( auto loaded = m_textureLoader.Load( g_FileSystem, asset ) )
                result = loaded.value();
            else
                result = std::unexpected( loaded.error() );

            break;
        }
        default:
            break;
    }

    if ( !result )
    {
        CMD_ERROR( "Failed to load asset [" + asset->Name + "]: " + result.error(), CMD::ASSETSYSTEM );
        return nullptr;
    }

    return result.value();
}

void AssetSystem::SearchAssets()
{
    m_assets.clear();

    const auto files = g_FileSystem->GetAllFiles();
    for ( const auto& file: files )
        DetectAndRegisterAsset( file );
}

void AssetSystem::DetectAndRegisterAsset( const FPath& file )
{
    auto ext = file.extension().string();
    std::ranges::transform( ext, ext.begin(), ::tolower );

    if ( ext == MATERIAL_EXTENSION )
        RegisterAsset( file, AssetType::MATERIAL );
    else if ( ext == SHADER_EXTENSION )
        RegisterAsset( file, AssetType::SHADER );
    else if ( ext == MODEL_EXTENSION )
        RegisterAsset( file, AssetType::MODEL );
    else
    {
        for ( const auto& imageExt: SUPPORTED_IMAGE_FORMATS )
            if ( ext == imageExt )
            {
                RegisterAsset( file, AssetType::TEXTURE );
                return;
            }
    }
}

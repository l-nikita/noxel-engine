#include <algorithm>
#include <cctype>
#include <filesystem>
#include <boost/uuid/generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>

#undef STB_IMAGE_WRITE_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION

#include "dbg/debug.hpp"
#include "assetsystem.hpp"
#include "filesystem/ifilesystem.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SysInit AssetSystem::Initialize()
{
    SearchAssets();

    CMD::RegisterCommand( "as_print_assets", "Prints assets", [this]
    {
        std::vector<std::string> assets;
        for ( const auto& asset: m_assets | std::views::values )
            assets.push_back( asset->Path.string() );

        std::ranges::sort( assets );
        for ( const auto& path: assets )
            CMD_MSG( path, CMD::ASSETSYSTEM );
    } );

    return {};
}

void AssetSystem::Shutdown()
{
    for ( auto asset: m_assets | std::views::values )
        delete asset;

    m_assets.clear();
}

const Assets& AssetSystem::GetAssets() const
{
    return m_assets;
}

std::optional<AssetId> AssetSystem::GetAsset( const FPath& path, AssetType type ) const
{
    const auto query = path.lexically_normal();
    const auto absoluteQuery = query.is_absolute()
                                   ? query
                                   : ( g_FileSystem->RootPath() / query ).lexically_normal();
    const auto queryName = query.filename().string();
    const auto queryStem = query.stem().string();

    for ( const auto& asset: m_assets | std::views::values )
    {
        if ( type == asset->Type && asset->Path == absoluteQuery )
            return asset->Id;
    }

    if ( query.has_parent_path() )
    {
        const auto queryPath = query.generic_string();
        for ( const auto& asset: m_assets | std::views::values )
            if ( type == asset->Type && asset->Path.generic_string().ends_with( queryPath ) )
                return asset->Id;
    } else
    {
        for ( const auto& asset: m_assets | std::views::values )
            if ( type == asset->Type && ( asset->Name == queryName || asset->Name == queryStem ) )
                return asset->Id;
    }

    return std::nullopt;
}

const Asset* AssetSystem::GetAsset( const AssetId& assetId ) const
{
    return m_assets.at( assetId );
}

void AssetSystem::RegisterAsset( const FPath& path, AssetType type )
{
    const auto relPath = g_FileSystem->Relative( path );

    if ( !g_FileSystem->Exists( relPath ) )
    {
        CMD_ERROR( "File not found: " + relPath.string(), CMD::ASSETSYSTEM );
        return;
    }

    for ( const auto asset: m_assets | std::views::values )
    {
        if ( asset->Path == relPath && asset->Type == type )
        {
            CMD_ERROR( "Asset with the same path and type is already registered", CMD::ASSETSYSTEM );
            return;
        }
    }

    AssetId assetId;
    auto metaPath = relPath.string() + META_EXTENSION;

    if ( !g_FileSystem->Exists( metaPath ) )
    {
        CMD_WARNING( "Meta file for asset " + relPath.string() + " is not found, creating a new one",
                     CMD::ASSETSYSTEM );

        const AssetId id = boost::uuids::random_generator()();
        assetId = id;

        nlohmann::json meta;
        meta[ "uuid" ] = boost::uuids::to_string( assetId );

        auto jsonStr = meta.dump();
        if ( !g_FileSystem->WriteText( metaPath, jsonStr ) )
        {
            CMD_ERROR( "Failed to create meta file: " + metaPath, CMD::ASSETSYSTEM );
            return;
        }
    } else
    {
        try
        {
            auto jsonStr = g_FileSystem->ReadText( metaPath );
            if ( !jsonStr.data() || jsonStr.empty() )
            {
                CMD_ERROR( "Asset meta is invalid: " + metaPath, CMD::ASSETSYSTEM );
                return;
            }

            nlohmann::json meta = nlohmann::json::parse( jsonStr );
            if ( !meta.contains( "uuid" ) || !meta[ "uuid" ].is_string() )
            {
                CMD_ERROR( "Meta file does not contain a valid uuid: " + metaPath, CMD::ASSETSYSTEM );
                return;
            }

            assetId = boost::uuids::uuid_from_string( meta[ "uuid" ].get<std::string>() );
        } catch ( const nlohmann::json::exception& e )
        {
            CMD_ERROR( "JSON parse error: " + std::string( e.what() ) + "; File: " + metaPath, CMD::ASSETSYSTEM );
            return;
        }
    }

    if ( assetId.is_nil() )
    {
        CMD_ERROR( "Asset id is invalid: " + relPath.string(), CMD::ASSETSYSTEM );
        return;
    }

    m_assets.emplace( assetId, new Asset{assetId, relPath.stem().string(), relPath, type} );
}

void* AssetSystem::LoadAssetImpl( const Asset* asset )
{
    if ( !asset )
    {
        CMD_ERROR( "Cannot load a null asset", CMD::ASSETSYSTEM );
        return nullptr;
    }

    const auto load = [&]() -> std::expected<void*, std::string>
    {
        switch ( asset->Type )
        {
            case AssetType::MATERIAL:
                return m_materialLoader.Load( g_FileSystem, asset );

            case AssetType::SHADER:
                return m_shaderLoader.Load( g_FileSystem, asset );

            case AssetType::TEXTURE:
                return m_textureLoader.Load( g_FileSystem, asset );

            default:
                return std::unexpected( "Unsupported asset type" );
        }
    };

    auto result = load();
    if ( !result )
    {
        FATAL_ERROR( "Failed to load asset [" + asset->Name + "]:\n" + result.error(), CMD::ASSETSYSTEM );
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

        for ( const auto& meshExt: SUPPORTED_MESH_FORMATS )
            if ( ext == meshExt )
            {
                RegisterAsset( file, AssetType::MESH );
                return;
            }
    }
}

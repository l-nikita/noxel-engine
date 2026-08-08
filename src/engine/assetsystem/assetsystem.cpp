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

#include "assetsystem/assetsystem.hpp"
#include "dbg/debug.hpp"
#include "filesystem/ifilesystem.hpp"

namespace assets
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SysInit AssetSystem::Initialize()
    {
        SearchAssets();
        SetupErrorAssets();

        cmd::RegisterCommand( "as_print_assets", "Prints assets", [this]
        {
            std::vector<std::string> assets;
            for ( const auto& asset: m_assets | std::views::values )
                assets.push_back( asset->Path.string() );

            std::ranges::sort( assets );
            for ( const auto& path: assets )
                CMD_MSG( path, cmd::ASSETSYSTEM );
        } );

        return {};
    }

    void AssetSystem::Shutdown()
    {
        for ( auto asset: m_assets | std::views::values )
            delete asset;

        m_assets.clear();
    }

    //-----------------------------------------------------------------------------
    const Assets& AssetSystem::GetAssets() const
    {
        return m_assets;
    }

    std::optional<Id> AssetSystem::GetAsset( const fs::Path& path, Type type ) const
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

    const Asset* AssetSystem::GetAsset( const Id& assetId ) const
    {
        return m_assets.at( assetId );
    }

    Id AssetSystem::GetErrorAsset( Type type ) const
    {
        if ( type == Type::TEXTURE )
            return ERROR_TEXTURE_ID;

        FATAL_ERROR( "Error asset for type '" + std::to_string( static_cast<int>(type) ) + "' is not found",
                     cmd::ASSETSYSTEM );

        return ERROR_TEXTURE_ID;
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void AssetSystem::RegisterAsset( const fs::Path& path, Type type )
    {
        const auto relPath = g_FileSystem->Relative( path );

        if ( !g_FileSystem->Exists( relPath ) )
        {
            CMD_ERROR( "File not found: " + relPath.string(), cmd::ASSETSYSTEM );
            return;
        }

        for ( const auto asset: m_assets | std::views::values )
        {
            if ( asset->Path == relPath && asset->Type == type )
            {
                CMD_ERROR( "Asset with the same path and type is already registered", cmd::ASSETSYSTEM );
                return;
            }
        }

        Id assetId;
        auto metaPath = relPath.string() + META_EXTENSION;

        if ( !g_FileSystem->Exists( metaPath ) )
        {
            CMD_WARNING( "Meta file for asset " + relPath.string() + " is not found, creating a new one",
                         cmd::ASSETSYSTEM );

            const Id id = boost::uuids::random_generator()();
            assetId = id;

            nlohmann::json meta;
            meta[ "uuid" ] = boost::uuids::to_string( assetId );

            auto jsonStr = meta.dump();
            if ( !g_FileSystem->WriteText( metaPath, jsonStr ) )
            {
                CMD_ERROR( "Failed to create meta file: " + metaPath, cmd::ASSETSYSTEM );
                return;
            }
        } else
        {
            try
            {
                auto jsonStr = g_FileSystem->ReadText( metaPath );
                if ( !jsonStr.data() || jsonStr.empty() )
                {
                    CMD_ERROR( "Asset meta is invalid: " + metaPath, cmd::ASSETSYSTEM );
                    return;
                }

                nlohmann::json meta = nlohmann::json::parse( jsonStr );
                if ( !meta.contains( "uuid" ) || !meta[ "uuid" ].is_string() )
                {
                    CMD_ERROR( "Meta file does not contain a valid uuid: " + metaPath, cmd::ASSETSYSTEM );
                    return;
                }

                assetId = boost::uuids::uuid_from_string( meta[ "uuid" ].get<std::string>() );
            } catch ( const nlohmann::json::exception& e )
            {
                CMD_ERROR( "JSON parse error: " + std::string( e.what() ) + "; File: " + metaPath, cmd::ASSETSYSTEM );
                return;
            }
        }

        if ( assetId.is_nil() )
        {
            CMD_ERROR( "Asset id is invalid: " + relPath.string(), cmd::ASSETSYSTEM );
            return;
        }

        m_assets.emplace( assetId, new Asset{assetId, relPath.stem().string(), relPath, type} );
    }

    //-----------------------------------------------------------------------------
    bool AssetSystem::IsSupportedImage( const fs::Path& file ) const
    {
        auto ext = file.extension().string();
        std::ranges::transform( ext, ext.begin(), ::tolower );

        for ( const auto& imageExt: SUPPORTED_IMAGE_FORMATS )
            if ( ext == imageExt )
                return true;

        return false;
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    IAssetData* AssetSystem::LoadDataImpl( const Asset* asset )
    {
        if ( !asset )
        {
            CMD_ERROR( "Cannot load a null asset", cmd::ASSETSYSTEM );
            return nullptr;
        }

        const auto load = [&]() -> std::expected<IAssetData*, std::string>
        {
            switch ( asset->Type )
            {
                case Type::MATERIAL:
                    return m_materialLoader.Load( g_FileSystem, asset );

                case Type::SHADER:
                    return m_shaderLoader.Load( g_FileSystem, asset );

                case Type::TEXTURE:
                    return m_textureLoader.Load( g_FileSystem, asset );

                default:
                    return std::unexpected( "Unsupported asset type" );
            }
        };

        auto result = load();
        if ( !result )
        {
            FATAL_ERROR( "Failed to load asset [" + asset->Name + "]:\n" + result.error(), cmd::ASSETSYSTEM );
            return nullptr;
        }

        return result.value();
    }

    void AssetSystem::SetupErrorAssets()
    {
        m_assets.emplace( ERROR_TEXTURE_ID, new Asset{ERROR_TEXTURE_ID, "error", "", Type::TEXTURE} );
    }

    void AssetSystem::SearchAssets()
    {
        const auto files = g_FileSystem->GetAllFiles();
        for ( const auto& file: files )
            DetectAndRegisterAsset( file );
    }

    //-----------------------------------------------------------------------------
    void AssetSystem::DetectAndRegisterAsset( const fs::Path& file )
    {
        auto ext = file.extension().string();
        std::ranges::transform( ext, ext.begin(), ::tolower );

        if ( ext == MATERIAL_EXTENSION )
            RegisterAsset( file, Type::MATERIAL );
        else if ( ext == SHADER_EXTENSION )
            RegisterAsset( file, Type::SHADER );
        else if ( ext == MODEL_EXTENSION )
            RegisterAsset( file, Type::MODEL );
        else
        {
            for ( const auto& imageExt: SUPPORTED_IMAGE_FORMATS )
                if ( ext == imageExt )
                {
                    RegisterAsset( file, Type::TEXTURE );
                    return;
                }

            for ( const auto& meshExt: SUPPORTED_MESH_FORMATS )
                if ( ext == meshExt )
                {
                    RegisterAsset( file, Type::MESH );
                    return;
                }
        }
    }
}

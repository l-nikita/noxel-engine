#pragma once

#include <optional>
#include <unordered_map>

#include "asset.hpp"
#include "common.hpp"

namespace assets
{
    //-----------------------------------------------------------------------------

    using Assets = std::unordered_map<Id, Asset*>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class IAssetSystem : public ISystem
    {
    public:
        virtual const Assets& GetAssets() const = 0;
        virtual std::optional<Id> GetAsset( const fs::Path& path, Type type ) const = 0;
        virtual const Asset* GetAsset( const Id& assetId ) const = 0;

        virtual void RegisterAsset( const fs::Path& path, Type type ) = 0;

        template<typename T>
        std::unique_ptr<T> LoadData( const Asset* asset )
        {
            return std::unique_ptr<T>(
                static_cast<T*>(LoadDataImpl( asset ))
            );
        }

        template<typename T>
        std::unique_ptr<T> LoadData( const Id& assetId )
        {
            return LoadData<T>( GetAsset( assetId ) );
        }

    protected:
        virtual IAssetData* LoadDataImpl( const Asset* asset ) = 0;
    };
}

extern assets::IAssetSystem* g_Assets;

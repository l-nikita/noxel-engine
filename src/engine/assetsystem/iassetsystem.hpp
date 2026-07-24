#pragma once

#include "asset.hpp"
#include "common.hpp"

//-----------------------------------------------------------------------------

using Assets = std::vector<Asset>;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IAssetSystem : public ISystem
{
public:
    virtual const Assets& GetAssets() const = 0;
    virtual const Asset* GetAsset( const FPath& path, AssetType type ) const = 0;

    virtual void RegisterAsset( const FPath& path, AssetType type ) = 0;

    template<typename T>
    T* LoadAsset( const Asset* asset )
    {
        return static_cast<T*>(LoadAssetImpl( asset ));
    }

protected:
    virtual void* LoadAssetImpl( const Asset* asset ) = 0;
};

extern IAssetSystem* g_Assets;

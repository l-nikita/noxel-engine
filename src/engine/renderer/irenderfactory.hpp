#pragma once

#include "idepthstencil.hpp"
#include "imaterial.hpp"
#include "imesh.hpp"
#include "irenderresource.hpp"
#include "irendertexture.hpp"
#include "iviewport.hpp"
#include "meshdata.hpp"
#include "assetsystem/asset.hpp"

//-----------------------------------------------------------------------------
// Кто Create... Тот делает Release...
//-----------------------------------------------------------------------------
class IRenderFactory
{
public:
    virtual ~IRenderFactory() = default;

    virtual ITexture* CreateTexture( const AssetId& assetId ) = 0;
    virtual ITexture* CreateTexture( const TextureDesc& desc ) = 0;
    virtual ITexture* CreateCubemap( const AssetId& assetId ) = 0;

    virtual IMaterial* CreateMaterial( const AssetId& assetId ) = 0;
    virtual IMaterial* CreateMaterial( std::string assetName ) = 0;

    virtual IMesh* CreateMesh( const MeshData& data ) = 0;
    virtual IShaderProgram* CreateShaderProgram( const AssetId& assetId ) = 0;

    virtual IRenderTexture* CreateRenderTexture( int width, int height, bool depth,
                                                 TextureFormat format = TextureFormat::RGBA32F ) = 0;
    virtual IDepthStencil* CreateDepthStencil( int width, int height, bool bindable = false ) = 0;

    virtual IViewport* CreateViewport( int x, int y, int width, int height ) = 0;

    virtual void Release( IRenderResource* resource ) = 0;
    virtual void Shutdown() = 0;
};

#pragma once

#include "imesh.hpp"
#include "imaterial.hpp"
#include "irendertexture.hpp"
#include "idepthstencil.hpp"
#include "ishader.hpp"
#include "iviewport.hpp"
#include "irenderresource.hpp"

#include "meshdata.hpp"

class ShaderAsset;
class TextureAsset;
class MaterialAsset;

//-----------------------------------------------------------------------------
//  то Create... тот должен Release...
//-----------------------------------------------------------------------------
class IRenderFactory 
{
public:
    virtual ITexture* CreateTexture(TextureAsset* asset) = 0;
    virtual ITexture* CreateTexture(const TextureDesc& desc) = 0;
    virtual ITexture* CreateCubemap(MaterialAsset* asset) = 0;

    virtual IMaterial* CreateMaterial(MaterialAsset* asset) = 0;
    virtual IMaterial* CreateMaterial(std::string assetName) = 0;

    virtual IMesh* CreateMesh(const MeshData& data) = 0;
    virtual IShaderProgram* CreateShaderProgram(ShaderAsset* asset) = 0;

    virtual IRenderTexture* CreateRenderTexture(int width, int height, bool depth, TextureFormat format = TextureFormat::RGBA32F) = 0;
    virtual IDepthStencil* CreateDepthStencil(int width, int height, bool bindable) = 0;

    virtual IViewport* CreateViewport(int x, int y, int width, int height) = 0;

    virtual void Release(IRenderResource* resource) = 0;
    virtual void Shutdown() = 0;
};
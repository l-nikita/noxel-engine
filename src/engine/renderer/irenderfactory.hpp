#pragma once

#include "imesh.hpp"
#include "imaterial.hpp"
#include "irendertexture.hpp"
#include "idepthstencil.hpp"
#include "ishader.hpp"
#include "iviewport.hpp"

#include "meshdata.hpp"

class ShaderAsset;
class TextureAsset;
class MaterialAsset;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IRenderFactory 
{
public:
    virtual IMesh* CreateMesh(const MeshData& data) = 0;
    virtual IShaderProgram* CreateShaderProgram(ShaderAsset* asset) = 0;
    virtual ITexture* CreateTexture(TextureAsset* asset) = 0;

    virtual IMaterial* CreateMaterial(MaterialAsset* asset) = 0;
    virtual IMaterial* CreateMaterial(std::string assetName) = 0;

    virtual IRenderTexture* CreateRenderTexture(int width, int height, bool depth) = 0;
    virtual IDepthStencil* CreateDepthStencil(IRenderTexture* rt) = 0;
    virtual IDepthStencil* CreateDepthStencil(int width, int height) = 0;
    virtual IViewport* CreateViewport(int x, int y, int width, int height) = 0;
};
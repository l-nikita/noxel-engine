#pragma once

#include <d3d11.h>
#include <string>
#include "renderer/irenderfactory.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11Factory : public IRenderFactory 
{
public:
    DX11Factory(ID3D11Device* device, ID3D11DeviceContext* context);

    IMesh* CreateMesh(const MeshData& data) override;
    IShaderProgram* CreateShaderProgram(ShaderAsset* asset) override;
    ITexture* CreateTexture(TextureAsset* asset) override;

    IMaterial* CreateMaterial(MaterialAsset* asset) override;
    IMaterial* CreateMaterial(std::string assetName) override;

    IRenderTexture* CreateRenderTexture(int width, int height, bool depth, TextureFormat format = TextureFormat::RGBA32F) override;
    IDepthStencil* CreateDepthStencil(IRenderTexture* rt) override;
    IDepthStencil* CreateDepthStencil(int width, int height) override;
    IViewport* CreateViewport(int x, int y, int width, int height) override;

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
};
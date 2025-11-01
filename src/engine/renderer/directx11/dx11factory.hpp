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
    ~DX11Factory() { Shutdown(); }

    ITexture* CreateTexture(TextureAsset* asset) override;
    ITexture* CreateTexture(const TextureDesc& desc) override;

    IMaterial* CreateMaterial(MaterialAsset* asset) override;
    IMaterial* CreateMaterial(std::string assetName) override;

    IMesh* CreateMesh(const MeshData& data) override;
    IShaderProgram* CreateShaderProgram(ShaderAsset* asset) override;

    IRenderTexture* CreateRenderTexture(int width, int height, bool depth, TextureFormat format = TextureFormat::RGBA32F) override;
    IDepthStencil* CreateDepthStencil(int width, int height) override;

    IViewport* CreateViewport(int x, int y, int width, int height) override;

    void Release(IRenderResource* resource) override;
    void Shutdown() override;

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    std::vector<IRenderResource*> m_resources;
};
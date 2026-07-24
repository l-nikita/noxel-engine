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
    DX11Factory( ID3D11Device* device, ID3D11DeviceContext* context );

    ~DX11Factory() override
    {
        Shutdown();
    }

    ITexture* CreateTexture( const Asset* asset ) override;
    ITexture* CreateTexture( const TextureDesc& desc ) override;
    ITexture* CreateCubemap( const Asset* asset ) override;

    IMaterial* CreateMaterial( const Asset* asset ) override;
    IMaterial* CreateMaterial( std::string assetName ) override;

    IMesh* CreateMesh( const MeshData& data ) override;
    IShaderProgram* CreateShaderProgram( const Asset* asset ) override;

    IRenderTexture* CreateRenderTexture( int width, int height, bool depth, TextureFormat format ) override;
    IDepthStencil* CreateDepthStencil( int width, int height, bool bindable ) override;

    IViewport* CreateViewport( int x, int y, int width, int height ) override;

    void Release( IRenderResource* resource ) override;
    void Shutdown() override;

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    std::vector<IRenderResource*> m_resources;
};

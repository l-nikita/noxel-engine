#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>

#include "common.hpp"
#include "renderer/irenderer.hpp"
#include "renderer/irendertexture.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11Renderer final : public IRenderer
{
public:
    SYSTEM_METADATA( RENDERER, "DX11Renderer" );

public:
    SysInit Initialize() override;
    void Shutdown() override;

    void OnWindowResize( int width, int height ) override;

    IRenderFactory* GetFactory() const override
    {
        return m_factory;
    }

    RenderCamera* GetCamera() const override
    {
        return m_camera;
    }

    void SetCamera( RenderCamera* camera ) override
    {
        m_camera = camera;
    }

    bool IsVSyncEnabled() override
    {
        return m_vsyncEnabled;
    }

    void SetVSync( bool value ) override;

    void SetPrimitiveTopology( PrimitiveTopology topology ) override;

    void CreateDevice();
    void CreateSwapChain();
    void WaitForFrame() override;
    void Swap() override;

    IRenderTexture* GetBackBufferRT() override
    {
        return m_backBuffer;
    }

    void CopyToBackBuffer( IRenderTexture* rt ) override;
    void SetRenderTextures( IRenderTexture* rts[ ], std::uint32_t num, IDepthStencil* depthStencil ) override;
    void BindRenderTextures( IRenderTexture* rts[ ], std::uint32_t num ) override;
    void DrawFullScreenTriangle() override;
    void SetViewport( IViewport* viewport ) override;

    ID3D11Device* GetDevice() const
    {
        return m_device;
    }

    ID3D11DeviceContext* GetContext() const
    {
        return m_context;
    }

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    IRenderTexture* m_backBuffer = nullptr;

    IDXGISwapChain2* m_swapChain = nullptr;
    HANDLE m_frameLatencyWaitableObject = nullptr;
    bool m_tearingSupported = false;

    IRenderFactory* m_factory = nullptr;
    RenderCamera* m_camera = nullptr;

    bool m_vsyncEnabled = true;
};

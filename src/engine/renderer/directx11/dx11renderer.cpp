#include <SDL3/SDL.h>
#include <glm/gtc/type_ptr.hpp>

#include "dx11renderer.hpp"
#include "dx11material.hpp"
#include "dx11mesh.hpp"
#include "dx11texture.hpp"
#include "dx11rendertexture.hpp"
#include "dx11factory.hpp"
#include "dx11viewport.hpp"

#include "iengine.hpp"
#include "window.hpp"
#include "renderer/imaterial.hpp"
#include "renderer/rendercamera.hpp"
#include "renderer/vertex.hpp"
#include "gui/igui.hpp"
#include "dbg/cmd/cmd.hpp"

// Shaders should be included here (somewhere). Compiler issue.
//#include "shaders/dx11shader_default.hpp"
//include "shaders/dx11shader_gizmos.hpp"
#include "shaders/dx11shader_geometrypass.hpp"
#include "shaders/dx11shader_lightingpass.hpp"
#include "shaders/dx11shader_skyboxpass.hpp"
#include "shaders/dx11shader_shadowspass.hpp"

#define INITGUID
#include <d3d11.h>
#include <dxgidebug.h>
#include <dxgi1_5.h>

namespace
{
    constexpr UINT cSwapChainBufferCount = 3;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SysInit DX11Renderer::Initialize()
{
    CreateDevice();
    CreateSwapChain();

    assert( m_device );
    assert( m_swapChain );

    //-------------------------------------------
    m_factory = new DX11Factory( m_device, m_context );

    ID3D11Texture2D* backBufferTex;
    HRESULT hr = m_swapChain->GetBuffer( 0, IID_PPV_ARGS( &backBufferTex ) );
    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: Failed to get back buffer.", CMD::RENDERING );

    // TODO: make from factory
    m_backBuffer = new DX11RenderTexture( m_device, m_context, backBufferTex, false, TextureFormat::RGBA32F );

    int width, height;
    g_Engine->GetWindow()->GetSize( &width, &height );

    CMD_INFO( "DX11: Renderer initialized", CMD::RENDERING );

    return {};
}

void DX11Renderer::Shutdown()
{
    if ( m_backBuffer )
        delete m_backBuffer, m_backBuffer = nullptr;

    if ( m_factory )
    {
        delete m_factory;
        m_factory = nullptr;
    }

    if ( m_context )
    {
        m_context->ClearState();
        m_context->Flush();
        m_context->Release();
        m_context = nullptr;
    }

    if ( m_swapChain )
        m_swapChain->Release(), m_swapChain = nullptr;

    if ( m_device )
        m_device->Release(), m_device = nullptr;

#ifdef _DEBUG
    IDXGIDebug1* pDebug = nullptr;
    if ( SUCCEEDED( DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug)) ) )
    {
        pDebug->ReportLiveObjects( DXGI_DEBUG_ALL,
                                   (DXGI_DEBUG_RLO_FLAGS)( DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_DETAIL ) );
        pDebug->Release();
    }
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::OnWindowResize( int width, int height )
{
    if ( m_backBuffer )
    {
        delete m_backBuffer;
        m_backBuffer = nullptr;
    }

    const UINT flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT |
                       ( m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0 );
    HRESULT hr = m_swapChain->ResizeBuffers( cSwapChainBufferCount, width, height,
                                             DXGI_FORMAT_R8G8B8A8_UNORM, flags );
    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: Failed to resize swap chain buffers.", CMD::RENDERING );

    ID3D11Texture2D* backBufferTex;
    hr = m_swapChain->GetBuffer( 0, IID_PPV_ARGS( &backBufferTex ) );
    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: Failed to get back buffer.", CMD::RENDERING );

    m_backBuffer = new DX11RenderTexture( m_device, m_context, backBufferTex, true, TextureFormat::RGBA32F );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::SetPrimitiveTopology( PrimitiveTopology topology )
{
    if ( topology == LINES )
        m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
    else if ( topology == TRIANGLES )
        m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::CreateDevice()
{
    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(
        nullptr, // adapter
        D3D_DRIVER_TYPE_HARDWARE, // driver type
        nullptr, // software
        flags, // flags
        nullptr, // feature levels
        0, // feature levels
        D3D11_SDK_VERSION, // version
        &m_device, // device
        &featureLevel, // feature level
        &m_context ); // device context

    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: D3D11CreateDevice failed.", CMD::RENDERING );
}

void DX11Renderer::CreateSwapChain()
{
    assert( m_device );

    auto window = g_Engine->GetWindow();
    HWND hwnd = (HWND)window->GetPointer();

    int width, height;
    window->GetSize( &width, &height );

    IDXGIDevice* dxgiDevice = nullptr;
    IDXGIAdapter* dxgiAdapter = nullptr;
    IDXGIFactory2* dxgiFactory = nullptr;
    m_device->QueryInterface( __uuidof(IDXGIDevice), (void**)&dxgiDevice );
    dxgiDevice->GetAdapter( &dxgiAdapter );
    dxgiAdapter->GetParent( __uuidof(IDXGIFactory2), (void**)&dxgiFactory );

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.Width = width;
    scDesc.Height = height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.Stereo = FALSE;
    scDesc.SampleDesc.Count = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = cSwapChainBufferCount;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Scaling = DXGI_SCALING_STRETCH;

    IDXGIFactory5* dxgiFactory5 = nullptr;
    if ( SUCCEEDED( dxgiFactory->QueryInterface( IID_PPV_ARGS( &dxgiFactory5 ) ) ) )
    {
        BOOL allowTearing = FALSE;
        if ( SUCCEEDED( dxgiFactory5->CheckFeatureSupport( DXGI_FEATURE_PRESENT_ALLOW_TEARING,
            &allowTearing, sizeof( allowTearing ) ) ) )
            m_tearingSupported = allowTearing == TRUE;

        dxgiFactory5->Release();
    }

    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT |
                   ( m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0 );

    IDXGISwapChain1* swapChain = nullptr;
    HRESULT hr = dxgiFactory->CreateSwapChainForHwnd( m_device, hwnd, &scDesc, nullptr, nullptr, &swapChain );
    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: CreateSwapChainForHwnd failed.", CMD::RENDERING );

    hr = swapChain->QueryInterface( IID_PPV_ARGS( &m_swapChain ) );
    swapChain->Release();
    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: IDXGISwapChain2 is not available.", CMD::RENDERING );

    hr = m_swapChain->SetMaximumFrameLatency( 1 );
    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: Failed to set maximum frame latency.", CMD::RENDERING );

    m_frameLatencyWaitableObject = m_swapChain->GetFrameLatencyWaitableObject();
    if ( !m_frameLatencyWaitableObject )
        FATAL_ERROR( "DX11: Failed to get frame latency waitable object.", CMD::RENDERING );

    dxgiDevice->Release();
    dxgiAdapter->Release();
    dxgiFactory->Release();
}

void DX11Renderer::WaitForFrame()
{
    if ( m_vsyncEnabled && m_frameLatencyWaitableObject )
        WaitForSingleObjectEx( m_frameLatencyWaitableObject, 1000, TRUE );
}

void DX11Renderer::SetVSync( const bool value )
{
    if ( m_vsyncEnabled == value )
        return;

    m_vsyncEnabled = value;

    if ( m_swapChain )
    {
        const UINT maximumFrameLatency = m_vsyncEnabled ? 1 : cSwapChainBufferCount;
        m_swapChain->SetMaximumFrameLatency( maximumFrameLatency );
    }
}

void DX11Renderer::Swap()
{
    const bool allowTearing = !m_vsyncEnabled && m_tearingSupported && !g_Engine->GetWindow()->IsInFullscreen();
    m_swapChain->Present( m_vsyncEnabled ? 1 : 0, allowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0 );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DX11Renderer::CopyToBackBuffer( IRenderTexture* rt )
{
    assert( rt );
    auto tex = static_cast<DX11Texture*>(rt->GetTexture());
    auto backTex = static_cast<DX11Texture*>(m_backBuffer->GetTexture());
    m_context->CopyResource( backTex->GetResource(), tex->GetResource() );
}

void DX11Renderer::SetRenderTextures( IRenderTexture* rts[ ], std::uint32_t num, IDepthStencil* depthStencil )
{
    ID3D11RenderTargetView* rtvs[ 8 ] = {nullptr};
    for ( auto i = 0; i < num; ++i )
        rtvs[ i ] = static_cast<ID3D11RenderTargetView*>(rts[ i ]->GetResource());

    ID3D11DepthStencilView* dsv = nullptr;
    if ( depthStencil )
        dsv = static_cast<ID3D11DepthStencilView*>(depthStencil->GetResource());

    m_context->OMSetRenderTargets( num, rtvs, dsv );
}

void DX11Renderer::BindRenderTextures( IRenderTexture* rts[ ], std::uint32_t num )
{
    DX11Texture* textures[ 8 ] = {nullptr};
    for ( auto i = 0; i < num; ++i )
        textures[ i ] = static_cast<DX11Texture*>(rts[ i ]->GetTexture());

    ID3D11ShaderResourceView* srvs[ 8 ];
    for ( auto i = 0; i < num; ++i )
        srvs[ i ] = textures[ i ]->GetSRV();

    m_context->PSSetShaderResources( 0, num, srvs );
}

void DX11Renderer::DrawFullScreenTriangle()
{
    g_Renderer->SetPrimitiveTopology( TRIANGLES );
    m_context->Draw( 3, 0 );
}

void DX11Renderer::SetViewport( IViewport* viewport )
{
    assert( viewport );
    static_cast<DX11Viewport*>(viewport)->Apply( m_context );
}

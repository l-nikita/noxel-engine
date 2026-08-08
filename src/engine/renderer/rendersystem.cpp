#include <cassert>
#include <DebugOutput.h>
#include <Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h>

#include "renderer/rendersystem.hpp"
#include "dbg/debug.hpp"
#include "engine/iengine.hpp"
#include "engine/window.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    void DiligentRendererCallback( DEBUG_MESSAGE_SEVERITY Severity,
                                   const Char* Message,
                                   const Char* Function,
                                   const Char* File,
                                   int Line )
    {
        auto msg = "Diligent: " + std::string( Message );

        switch ( Severity )
        {
            case DEBUG_MESSAGE_SEVERITY_INFO:
                CMD_INFO( msg, cmd::RENDERING );
                break;
            case DEBUG_MESSAGE_SEVERITY_WARNING:
                CMD_WARNING( msg, cmd::RENDERING );
                break;
            case DEBUG_MESSAGE_SEVERITY_ERROR:
                CMD_ERROR( msg, cmd::RENDERING );
                break;
            case DEBUG_MESSAGE_SEVERITY_FATAL_ERROR:
                FATAL_ERROR( msg, cmd::RENDERING );
                break;
        }
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    RenderSystem::RenderSystem( Window* window )
        : m_window( window )
    {
        assert( m_window );
    }

    SysInit RenderSystem::Initialize()
    {
        SetDebugMessageCallback( DiligentRendererCallback );

        if ( auto result = CreateDeviceAndSwapChain(); !result )
            return result;

        m_window->SubscribeResize( [this]( const WindowSize& size )
        {
            auto [ wW, wH ] = size;

            if ( m_swapChain )
                m_swapChain->Resize( wW, wH );
        } );

        return {};
    }

    void RenderSystem::Shutdown()
    {
        if ( m_context )
            m_context->Flush();
    }

    Window* RenderSystem::GetWindow() const
    {
        return m_window;
    }

    void RenderSystem::Present()
    {
        if ( m_swapChain )
            m_swapChain->Present();
    }

    RefCntAutoPtr<IRenderDevice> RenderSystem::GetDevice()
    {
        return m_device;
    }

    RefCntAutoPtr<IDeviceContext> RenderSystem::GetContext()
    {
        return m_context;
    }

    RefCntAutoPtr<ISwapChain> RenderSystem::GetSwapChain()
    {
        return m_swapChain;
    }

    SysInit RenderSystem::CreateDeviceAndSwapChain()
    {
        auto [ wW, wH ] = m_window->GetSize();

        SwapChainDesc scDesc;
        scDesc.ColorBufferFormat = TEX_FORMAT_RGBA8_UNORM;
        scDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;
        scDesc.BufferCount = 2;
        scDesc.Width = wW;
        scDesc.Height = wH;
        scDesc.PreTransform = SURFACE_TRANSFORM_IDENTITY;
        scDesc.Usage = SWAP_CHAIN_USAGE_RENDER_TARGET;

#if PLATFORM_WIN32
        EngineD3D11CreateInfo engineCI;
        engineCI.Features.SeparablePrograms = DEVICE_FEATURE_STATE_OPTIONAL;

        IEngineFactoryD3D11* factoryD3D11 = LoadAndGetEngineFactoryD3D11();

        factoryD3D11->CreateDeviceAndContextsD3D11( engineCI, &m_device, &m_context );
        Win32NativeWindow Window{m_window->GetPointer()};

        factoryD3D11->CreateSwapChainD3D11( m_device, m_context, scDesc, FullScreenModeDesc{}, Window,
                                            &m_swapChain );
#endif

        return {};
    }
}

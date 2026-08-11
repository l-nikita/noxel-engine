#include <cassert>
#include <DebugOutput.h>
#include <Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h>
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>

#include "renderer/rendersystem.hpp"

#include "factory.hpp"
#include "assetsystem/iassetsystem.hpp"
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

        m_factory = std::make_unique<Factory>( this );

        CreateDefaultTextures();

        return {};
    }

    void RenderSystem::Shutdown()
    {
        if ( m_context )
            m_context->Flush();
    }

    //-----------------------------------------------------------------------------
    Window* RenderSystem::GetWindow() const
    {
        return m_window;
    }

    void RenderSystem::Present()
    {
        if ( m_swapChain )
            m_swapChain->Present();
    }

    //-----------------------------------------------------------------------------
    IRenderDevice* RenderSystem::GetDevice() const
    {
        return m_device.RawPtr();
    }

    IDeviceContext* RenderSystem::GetContext() const
    {
        return m_context.RawPtr();
    }

    ISwapChain* RenderSystem::GetSwapChain() const
    {
        return m_swapChain.RawPtr();
    }

    IEngineFactory* RenderSystem::GetEngineFactory() const
    {
        return m_engineFactory.RawPtr();
    }

    Factory* RenderSystem::GetFactory() const
    {
        return m_factory.get();
    }

    const DefaultTextures& RenderSystem::GetDefaultTextures()
    {
        return m_defTextures;
    }

    //-----------------------------------------------------------------------------
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

        m_engineFactory = factoryD3D11;
#endif

        return {};
    }

    //-----------------------------------------------------------------------------
    void RenderSystem::CreateDefaultTextures()
    {
        m_defTextures = {
            .Black = m_factory->CreateSolidTexture( {0, 0, 0, 255}, "Black" ),
            .White = m_factory->CreateSolidTexture( {255, 255, 255, 255}, "White" ),
            .NeutralNormal = m_factory->CreateSolidTexture( {128, 128, 255, 255}, "Neutral Normal" ),
        };
    }
}

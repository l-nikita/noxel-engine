#include <unordered_set>

#include "engine.hpp"
#include "assetsystem/assetsystem.hpp"
#include "dbg/debug.hpp"
#include "filesystem/filesystem.hpp"
#include "gui/gui.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "renderer/deferred_render_pipeline.hpp"
#include "renderer/gizmos.hpp"
#include "renderer/directx11/dx11renderer.hpp"

IFileSystem* g_FileSystem = nullptr;

IAssetSystem* g_Assets = nullptr;
IInput* g_Input = nullptr;
IPhysics* g_Physics = nullptr;
IRenderer* g_Renderer = nullptr;
IRenderPipeline* g_RenderPipeline = nullptr;
IGui* g_Gui = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::InitializeSystems()
{
    g_FileSystem = m_fileSystem = new ::FileSystem();
    g_Assets = m_assetSystem = new ::AssetSystem();
    g_Input = m_input = new ::Input();
    g_Physics = m_physics = new JoltPhysics();

    switch ( g_Engine->GetGraphicsAPI() )
    {
        case GraphicsAPI::OPENGL:
            FATAL_ERROR( "OpenGL is NOT supported!", CMD::ENGINE );
        case GraphicsAPI::DIRECTX11:
#ifdef DX11
            g_Renderer = m_renderer = new DX11Renderer();
#else
            FATAL_ERROR( "DirectX 11 is NOT supported on your platform!", CMD::ENGINE );
#endif
            break;
        case GraphicsAPI::VULKAN:
            assert( false );
        default:
            FATAL_ERROR( "No graphics API!", CMD::ENGINE );
    }

    g_RenderPipeline = m_renderPipeline = new DeferredRenderPipeline();
    g_Gui = m_gui = new ::Gui();

    m_systems.append_range( std::initializer_list<ISystem*>{
        m_fileSystem,
        m_assetSystem,
        m_input,
        m_physics,
        m_renderer,
        m_renderPipeline,
        m_gui
    } );

    std::unordered_set<SysId> initialized;

    for ( auto system: m_systems )
    {
        const auto& [ Id, Name, Dependencies ] = system->GetMetadata();

        for ( auto dep: Dependencies )
        {
            if ( !initialized.contains( dep ) )
                FATAL_ERROR( "Dependency is not initialized for system: " + std::string(Name), CMD::ENGINE );
        }

        const auto result = system->Initialize();
        if ( !result )
            FATAL_ERROR( "Failed to initialize: " + result.error(), CMD::ENGINE );

        CMD_INFO( std::string(Name) + " initialized", CMD::ENGINE );

        initialized.insert( Id );
    }

    Gizmos::Init();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::ShutdownSystems()
{
    Gizmos::Shutdown();

    for ( auto it = m_systems.rbegin(); it != m_systems.rend(); ++it )
    {
        ( *it )->Shutdown();
        delete *it;
    }

    m_systems.clear();

    g_FileSystem = m_fileSystem = nullptr;
    g_Assets = m_assetSystem = nullptr;
    g_Input = m_input = nullptr;
    g_Physics = m_physics = nullptr;
    g_Renderer = m_renderer = nullptr;
    g_RenderPipeline = m_renderPipeline = nullptr;
    g_Gui = m_gui = nullptr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
IFileSystem& Engine::FileSystem() const
{
    return *m_fileSystem;
}

IAssetSystem& Engine::AssetSystem() const
{
    return *m_assetSystem;
}

IInput& Engine::Input() const
{
    return *m_input;
}

IPhysics& Engine::Physics() const
{
    return *m_physics;
}

IRenderer& Engine::Renderer() const
{
    return *m_renderer;
}

IRenderPipeline& Engine::RenderPipeline() const
{
    return *m_renderPipeline;
}

IGui& Engine::Gui() const
{
    return *m_gui;
}

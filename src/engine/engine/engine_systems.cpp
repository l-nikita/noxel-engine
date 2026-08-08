#include <unordered_set>

#include "assetsystem/assetsystem.hpp"
#include "dbg/debug.hpp"
#include "engine/engine.hpp"
#include "filesystem/filesystem.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "renderer/rendersystem.hpp"
#include "renderer/deferred/deferred_render_pipeline.hpp"

IFileSystem* g_FileSystem = nullptr;
IAssetSystem* g_Assets = nullptr;
IInput* g_Input = nullptr;
IPhysics* g_Physics = nullptr;
RenderSystem* g_Render = nullptr;
IRenderPipeline* g_RenderPipeline = nullptr;
IGui* g_Gui = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::InitializeSystems()
{
    g_FileSystem = m_fileSystem = new fs::FileSystem();
    g_Assets = m_assetSystem = new assets::AssetSystem();
    g_Input = m_input = new input::Input();
    g_Physics = m_physics = new phys::JoltPhysics();
    g_Render = m_render = new render::RenderSystem( GetPrimaryWindow() );
    g_RenderPipeline = m_renderPipeline = new render::deferred::DeferredRenderPipeline();

    m_systems.append_range( std::initializer_list<ISystem*>{
        m_fileSystem,
        m_assetSystem,
        m_input,
        m_physics,
        m_render,
        m_renderPipeline,
    } );

    std::unordered_set<SysId> initialized;

    for ( auto system: m_systems )
    {
        if ( !system )
            continue;

        const auto& [ Id, Name, Dependencies ] = system->GetMetadata();

        for ( auto dep: Dependencies )
        {
            if ( !initialized.contains( dep ) )
                FATAL_ERROR( "Dependency is not initialized for system: " + std::string(Name), cmd::ENGINE );
        }

        const auto result = system->Initialize();
        if ( !result )
            FATAL_ERROR( "Failed to initialize: " + result.error(), cmd::ENGINE );

        CMD_INFO( std::string(Name) + " initialized", cmd::ENGINE );

        initialized.insert( Id );
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::ShutdownSystems()
{
    for ( auto it = m_systems.rbegin(); it != m_systems.rend(); ++it )
    {
        auto sys = *it;
        if ( !sys )
            continue;

        sys->Shutdown();
        delete *it;
    }

    m_systems.clear();

    g_FileSystem = m_fileSystem = nullptr;
    g_Assets = m_assetSystem = nullptr;
    g_Input = m_input = nullptr;
    g_Physics = m_physics = nullptr;
    g_Render = m_render = nullptr;
    g_RenderPipeline = m_renderPipeline = nullptr;
}

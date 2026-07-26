#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#ifdef DX11
#include "renderer/directx11/dx11renderer.hpp"
#else
// opengl or vulkan
#endif
#include "engine.hpp"
#include "window.hpp"
#include "assetsystem/assetsystem.hpp"
#include "dbg/debug.hpp"
#include "dbg/cmd/cmd.hpp"
#include "physics/physics.hpp"
#include "renderer/irenderpipeline.hpp"

#include "player.hpp"

IEngine* g_Engine = nullptr;
Player* g_Player = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Initialize()
{
    CMD_INFO( "Hello! I am engine!", CMD::ENGINE );

#ifdef _DEBUG
    CMD_INFO( "Built in DEBUG mode", CMD::ENGINE );
#else
    CMD_INFO( "Built in RELEASE mode", CMD::ENGINE );
#endif

    if ( !SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) )
        FATAL_ERROR( "SDL_Init Error: " + std::string(SDL_GetError()), CMD::ENGINE );

    SetupWindow();

#ifdef WIN32
    m_graphicsAPI = GraphicsAPI::DIRECTX11;
#else
    m_graphicsAPI = GraphicsAPI::VULKAN;
#endif

    InitializeSystems();

    CMD::CMD::Get().Setup();

    CMD::RegisterCommand( "quit", "", []
    {
        g_Engine->Quit();
    } );

    if ( g_Renderer )
        g_Player = new Player( g_Renderer->GetCamera() );

    CMD_INFO( "Engine initialized successfully", CMD::ENGINE );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Shutdown()
{
    Quit();

    if ( g_Player )
        delete g_Player, g_Player = nullptr;

    ShutdownSystems();

    if ( m_window )
        delete m_window, m_window = nullptr;

    SDL_Quit();
    CMD_INFO( "Engine stopped", CMD::ENGINE );
}

bool Engine::IsRunning()
{
    return m_isRunning;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Window* Engine::GetWindow()
{
    return m_window;
}

void Engine::OnWindowResize()
{
    auto [ wW, wH ] = GetWindow()->GetSize();

    if ( g_RenderPipeline )
        g_RenderPipeline->OnWindowResize( wW, wH );
}

std::chrono::duration<float> Engine::DeltaDuration() const
{
    return m_deltaTime;
}

float Engine::DeltaTime()
{
    return m_deltaTime.count();
}

float Engine::FixedAlpha()
{
    constexpr float cFixedDeltaTime = 1.0f / 60.0f;
    return std::clamp( m_accumulator / cFixedDeltaTime, 0.0f, 1.0f );
}

std::chrono::duration<float> Engine::ElapsedDuration() const
{
    return m_elapsedTime;
}

float Engine::ElapsedTime()
{
    return m_elapsedTime.count();
}

float Engine::GetFPS()
{
    return 1.0f / DeltaTime();
}

GraphicsAPI Engine::GetGraphicsAPI()
{
    return m_graphicsAPI;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::SetupWindow()
{
    m_window = new Window( ENGINE_NAME, 1920, 1080 );
}

#include <SDL3/SDL.h>

#include <chrono>
#include <iostream>
#include <string>
#ifdef DX11
#include "renderer/directx11/dx11renderer.hpp"
#else
// opengl or vulkan
#endif
#include "engine.hpp"
#include "event_handler.hpp"
#include "window.hpp"
#include "assetsystem/asset_system.hpp"
#include "dbg/debug.hpp"
#include "dbg/profiler.hpp"
#include "dbg/cmd/cmd.hpp"
#include "gui/gui.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "renderer/deferred_render_pipeline.hpp"
#include "renderer/gizmos.hpp"

#include "player.hpp"
#include "gui/imgui/debug_menu/debug_menu.hpp"

IEngine* g_Engine = nullptr;
IInput* g_Input = nullptr;
IAssetSystem* g_Assets = nullptr;
IPhysics* g_Physics = nullptr;
IRenderer* g_Renderer = nullptr;
IRenderPipeline* g_RenderPipeline = nullptr;
IGui* g_Gui = nullptr;

Player* g_Player = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::Init()
{
    CMD_INFO("Hello! I am engine!", CMD::ENGINE);

#ifdef _DEBUG
    CMD_INFO("Built in DEBUG mode.", CMD::ENGINE);
#else
    CMD_INFO("Built in RELEASE mode.", CMD::ENGINE);
#endif

    if ( !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) )
    {
        CMD_ERROR("SDL_Init Error: " + std::string(SDL_GetError()), CMD::ENGINE);
        return false;
    }

    InitWindow();

#ifdef WIN32
    m_graphicsAPI = GraphicsAPI::DIRECTX11;
#else
    m_graphicsAPI = GraphicsAPI::VULKAN;
#endif

    g_Assets = new AssetSystem();
    g_Assets->Init();

    g_Input = new Input();
    g_Input->Init();

    if ( g_Engine->GetGraphicsAPI() == GraphicsAPI::VULKAN )
    {
        assert(false);
    }
#ifdef DX11
    else if ( g_Engine->GetGraphicsAPI() == GraphicsAPI::DIRECTX11 )
    {
        g_Renderer = new DX11Renderer();
    }
#endif

    if ( g_Renderer )
    {
        g_Renderer->Init();

        g_RenderPipeline = new DeferredRenderPipeline();
        g_RenderPipeline->Init();

        g_Gui = new Gui();
        g_Gui->Init();

        Gizmos::Init();
    }

    CMD::CMD::GetInstance().Setup();

    g_Physics = new JoltPhysics();
    g_Physics->Init();

    if ( g_Renderer )
        g_Player = new Player(g_Renderer->GetCamera());

    CMD_INFO("Engine initialized successfully.", CMD::ENGINE);
    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::InitWindow()
{
    m_window = new Window(ENGINE_NAME, 1920, 1080);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Run()
{
    using namespace std::chrono;
    auto lastTime = high_resolution_clock::now();
    const auto startTime = high_resolution_clock::now();

    m_isRunning = true;

    while ( m_isRunning )
    {
        auto currentTime = high_resolution_clock::now();
        m_deltaTime = duration<float>(currentTime - lastTime);
        lastTime = currentTime;
        m_elapsedTime = duration<float>(currentTime - startTime);

        PROF_START("Engine::Update", "Engine");
        Update();
        PROF_END("Engine::Update");

        PROF_START("Engine::Render", "Engine");
        Render();
        PROF_END("Engine::Render");
    }

    Shutdown();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Stop()
{
    m_isRunning = false;
}

//-----------------------------------------------------------------------------
// Immediately stops engine
//-----------------------------------------------------------------------------
void Engine::Shutdown()
{
    Stop();

    if ( g_Renderer )
        Gizmos::Shutdown();

    if ( g_Gui )
        delete g_Gui, g_Gui = nullptr;

    if ( g_Physics )
        delete g_Physics, g_Physics = nullptr;

    if ( g_Assets )
        delete g_Assets, g_Assets = nullptr;

    if ( g_Input )
        delete g_Input, g_Input = nullptr;

    if ( g_RenderPipeline )
        delete g_RenderPipeline, g_RenderPipeline = nullptr;

    if ( g_Renderer )
        delete g_Renderer, g_Renderer = nullptr;

    if ( m_window )
        delete m_window, m_window = nullptr;

    SDL_Quit();
    CMD_INFO("Engine stopped.", CMD::ENGINE);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Update()
{
    EventHandler::GetInstance().HandleEvents();

    if ( g_Player )
        g_Player->Update();

    g_Physics->Update();

    const bool isGrave = g_Input->IsKeyPressed(SDL_SCANCODE_GRAVE);
    const bool isF3 = g_Input->IsKeyPressed(SDL_SCANCODE_F3);
    const bool isF11 = g_Input->IsKeyPressed(SDL_SCANCODE_F11);

    const auto gui = dynamic_cast<Gui*>(g_Gui);
    if ( isGrave )
        gui->m_DebugMenu->m_IsVisible = !gui->m_DebugMenu->m_IsVisible;
    if ( isF3 )
        g_Input->SetRelativeMouseMode(!g_Input->GetRelativeMouseMode());
    else if ( isF11 )
        GetWindow()->ToggleFullscreen();

    g_Input->Reset();
}

void Engine::Render()
{
    if ( GetWindow()->IsMinimized() )
        return;

    if ( g_RenderPipeline )
        g_RenderPipeline->Render();
}

void Engine::DrawPlayer()
{
    g_Player->Draw();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::OnWindowResize()
{
    int width, height;
    GetWindow()->GetSize(&width, &height);

    if ( g_RenderPipeline )
        g_RenderPipeline->OnWindowResize(width, height);
}

#include <SDL3/SDL.h>

#include <iostream>
#include <string>
#include <chrono>

#include "renderer/directx11/dx11renderer.hpp"
#include "renderer/deferred_render_pipeline.hpp"
#include "renderer/render_camera.hpp"
#include "renderer/gizmos.hpp"
#include "event_handler.hpp"
#include "input/input.hpp"
#include "gui/gui.hpp"
#include "gui/panel.hpp"
#include "gui/imgui/debug_menu/debug_menu.hpp"
#include "assetsystem/asset_system.hpp"
#include "physics/physics.hpp"
#include "networking/networking.hpp"
#include "server/server.hpp"
#include "client/client.hpp"
#include "dbg/profiler.hpp"
#include "dbg/debug.hpp"
#include "dbg/cmd/cmd.hpp"
#include "engine.hpp"
#include "window.hpp"

IEngine* g_Engine = nullptr;
IInput* g_Input = nullptr;
IAssetSystem* g_Assets = nullptr;
IPhysics* g_Physics = nullptr;
IRenderer* g_Renderer = nullptr;
IRenderPipeline* g_RenderPipeline = nullptr;
IGui* g_Gui = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Update()
{
    EventHandler::GetInstance().HandleEvents();

    //g_Physics->Update();
    
    auto camera = g_Renderer->GetCamera();
    if (camera)
        camera->Update();

    bool isGrave = g_Input->IsKeyPressed(SDL_SCANCODE_GRAVE);
    bool isF3 = g_Input->IsKeyPressed(SDL_SCANCODE_F3);
    bool isF11 = g_Input->IsKeyPressed(SDL_SCANCODE_F11);

    auto gui = static_cast<Gui*>(g_Gui);
    if (isGrave)
        gui->m_DebugMenu->m_IsVisible = !gui->m_DebugMenu->m_IsVisible;
    if (isF3)
        g_Input->SetRelativeMouseMode(!g_Input->GetRelativeMouseMode());
    else if (isF11)
        GetWindow()->ToggleFullscreen();

    g_Input->Reset();
}

void Engine::Render()
{
    if (GetWindow()->IsMinimized())
        return;

    g_RenderPipeline->Render();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Run()
{
    using namespace std::chrono;
    auto lastTime = high_resolution_clock::now();
    auto startTime = high_resolution_clock::now();

    m_isRunning = true;

    while (m_isRunning)
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

    g_RenderPipeline->Shutdown();
    g_Gui->Shutdown();

    delete g_Assets;
    delete g_Input;
    delete g_RenderPipeline;
    delete g_Renderer;
    delete g_Gui;

    Gizmos::Shutdown();

    GetWindow()->Destroy();
    delete m_window;

    SDL_Quit();
    CMD_INFO("Engine stopped.", CMD::ENGINE);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::InitWindow()
{
    m_window = new Window(ENGINE_NAME, 1920, 1080);
    return m_window != nullptr;
}

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

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        CMD_ERROR("SDL_Init Error: " + std::string(SDL_GetError()), CMD::ENGINE);
        return false;
    }

    if (!InitWindow()) 
        return false;

    g_Assets = new AssetSystem();
    g_Assets->Init();

    g_Input = new Input();
    g_Input->Init();

    if (g_Engine->GetGraphicsAPI() == DIRECTX11)
        g_Renderer = new DX11Renderer();
    else
        FATAL_ERROR("Invalid graphics API.", CMD::ENGINE);

    g_Renderer->Init();

    g_RenderPipeline = new DeferredRenderPipeline();
    g_RenderPipeline->Init();

    g_Gui = new Gui();
    g_Gui->Init();

    Gizmos::Init();
    CMD::CMD::GetInstance().Setup();

    CMD_INFO("Engine initialized successfully.", CMD::ENGINE);
    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::OnWindowResize()
{
    int width, height;
    GetWindow()->GetSize(&width, &height);
    g_RenderPipeline->OnWindowResize(width, height);
}
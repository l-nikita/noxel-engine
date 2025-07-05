#include <SDL3/SDL.h>

#include <iostream>
#include <string>
#include <chrono>

#include "renderer/directx11/dx11renderer.hpp"
#include "renderer/render_camera.hpp"
#include "engine.hpp"
#include "assetsystem/assets_system.hpp"
#include "physics/physics.hpp"
#include "input/input.hpp"
#include "gui/gui.hpp"
#include "gui/panel.hpp"
#include "gui/imgui/debug_menu/debug_menu.hpp"
#include "common/profiler.hpp"
#include "event_handler.hpp"
#include "cmd/cmd.hpp"
#include "networking/networking.hpp"
#include "server/server.hpp"
#include "client/client.hpp"

IEngine* g_Engine = nullptr;
IInput* g_Input = nullptr;
IAssetsSystem* g_Assets = nullptr;
IPhysics* g_Physics = nullptr;
IRenderer* g_Renderer = nullptr;
IGui* g_Gui = nullptr;
//IEntities* g_Entities = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Update()
{
    EventHandler::GetInstance().HandleEvents();

    //g_Entities->Update();
    g_Physics->Update();
    
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
        ToggleFullscreen();

    g_Input->Reset();
}

void Engine::Render()
{
    Uint32 flags = SDL_GetWindowFlags(m_window);
    if (flags & SDL_WINDOW_MINIMIZED)
        return;

    g_Renderer->Render();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Run()
{
    using namespace std::chrono;
    auto lastTime = high_resolution_clock::now();
    auto startTime = high_resolution_clock::now();

    Profiler updateProfiler("Update");
    Profiler renderProfiler("Render");

    m_isRunning = true;

    while (m_isRunning)
    {
        auto currentTime = high_resolution_clock::now();
        m_deltaTime = duration<float, std::milli>(currentTime - lastTime);
        lastTime = currentTime;
        m_elapsedTime = duration<float, std::milli>(currentTime - startTime);

        updateProfiler.Start();
        Update();
        updateProfiler.End();

        renderProfiler.Start();
        Render();
        renderProfiler.End();
    }

    Shutdown();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Stop()
{
    if (!m_isRunning) return;
    m_isRunning = false;
}

//-----------------------------------------------------------------------------
// Immediately stops engine
//-----------------------------------------------------------------------------
void Engine::Shutdown()
{
    if (!m_isRunning) return;
    Stop();

    //g_Entities->Shutdown();
    g_Physics->Shutdown();
    Networking::GetInstance().Shutdown();

    g_Renderer->Shutdown();
    g_Gui->Shutdown();

    delete g_Assets;
    delete g_Input;
    delete g_Physics;
    delete g_Renderer;
    delete g_Gui;
    //delete g_Entities;

    if(GetWindow())
        SDL_DestroyWindow(GetWindow());

    SDL_Quit();
    CMD_LOG_INFO("Engine stopped.");
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::InitWindow()
{
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if (GetGraphicsAPI() == DIRECTX11)
        m_window = SDL_CreateWindow(ENGINE_NAME, 1920, 1080, SDL_WINDOW_RESIZABLE);
    else
        throw std::runtime_error("Unknown graphics API!");

    if (!GetWindow())
    {
        CMD_LOG("SDL_CreateWindow Error: " + std::string(SDL_GetError()), ERROR);
        Shutdown();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::Init()
{
    CMD_LOG("Hello! I am engine!", INFO);

#ifdef _DEBUG
    CMD_LOG("Built in DEBUG mode.", INFO);
#else
    CMD_LOG("Built in RELEASE mode.", INFO);
#endif

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        CMD_LOG("SDL_Init Error: " + std::string(SDL_GetError()), ERROR);
        return false;
    }

    if (!InitWindow()) return false;

    g_Assets = new AssetsSystem();
    if (!g_Assets->Init()) return false;

    g_Input = new Input();
    if (!g_Input->Init()) return false;

    g_Physics = new Physics();
    if(!g_Physics->Init()) return false;

    if (GetGraphicsAPI() == DIRECTX11)
    {
        g_Renderer = new DX11Renderer();
        if (!g_Renderer->Init()) return false;
    }
    else
        throw std::runtime_error("Unknown graphics API!");

    //g_Gui = new Gui();
    //if (!g_Gui->Init()) return false;
       
    //if (!Gizmos::GetInstance().Init()) return false;
    if (!Networking::GetInstance().Init()) return false;

    //g_Entities = new Entities();
    //if (!g_Entities->Init()) return false;

    CMD::GetInstance().Setup();
    CMD_LOG("Engine initialized successfully.", INFO);

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void* Engine::GetNativeWindowHandle()
{
#ifdef SDL_PLATFORM_WIN32
    return SDL_GetPointerProperty(SDL_GetWindowProperties(GetWindow()), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
    return nullptr;
#endif
}

void Engine::SetFullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(GetWindow(), fullscreen);
}

void Engine::ToggleFullscreen()
{
    SetFullscreen(!IsInFullscreen());
}

bool Engine::IsInFullscreen()
{
    Uint32 flags = SDL_GetWindowFlags(GetWindow());
    return (flags & SDL_WINDOW_FULLSCREEN);
}

void Engine::OnWindowResize()
{
    int width, height;
    SDL_GetWindowSizeInPixels(GetWindow(), &width, &height);
    g_Renderer->OnWindowResize(width, height);
}
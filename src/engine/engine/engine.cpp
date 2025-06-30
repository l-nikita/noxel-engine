#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <iostream>
#include <string>
#include <chrono>

#include "engine.hpp"
#include "gui/gui.hpp"
#include "assetsystem/assets_system.hpp"
#include "physics/physics.hpp"
#include "input.hpp"

#include "renderer/renderer.hpp"
#include "renderer/gizmos.hpp"
#include "renderer/render_camera.hpp"

#include "entities/entities.hpp"
#include "gui/imgui/debug_menu/debug_menu.hpp"
#include "gui/panel.hpp"
#include "common/profiler.hpp"
#include "event_handler.hpp"
#include "cmd/cmd.hpp"
#include "networking.hpp"
#include "server/server.hpp"
#include "client/client.hpp"

IEngine* g_Engine = nullptr;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Update()
{
    EventHandler::GetInstance().HandleEvents();

    Entities::GetInstance().Update();
    Physics::GetInstance().Update();

    auto camera = m_renderer->GetCamera();
    if (camera)
        camera->Update();

    bool isGrave = Input::GetInstance().IsKeyPressed(SDL_SCANCODE_GRAVE);
    bool isF3 = Input::GetInstance().IsKeyPressed(SDL_SCANCODE_F3);
    bool isF11 = Input::GetInstance().IsKeyPressed(SDL_SCANCODE_F11);

    //if (isGrave)
    //    m_DebugMenu->m_IsVisible = !m_DebugMenu->m_IsVisible;
    if (isF3)
        Input::GetInstance().SetRelativeMouseMode(!Input::GetInstance().GetRelativeMouseMode());
    else if (isF11)
        ToggleFullscreen();

    Input::GetInstance().Reset();
}

void Engine::Render()
{
    m_renderer->Render();
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
    Profiler swapBuffers("SwapBuffers");

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

        if (!m_isRunning)
            break;

        renderProfiler.Start();
        Render();
        renderProfiler.End();

        //move to renderer
        swapBuffers.Start();
        SDL_GL_SwapWindow(m_window);
        swapBuffers.End();
    }

    Shutdown();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Shutdown()
{
    if (!m_isRunning) return;

    m_isRunning = false;

    Entities::GetInstance().Shutdown();
    Physics::GetInstance().Shutdown();
    Networking::GetInstance().Shutdown();

    m_renderer->Shutdown();
    m_gui->Shutdown();

    delete m_renderer;
    delete m_gui;

    if(m_window)
        SDL_DestroyWindow(m_window);

    SDL_Quit();
    CMD_LOG_INFO("Engine stopped.");
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::CreateWindow()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window = SDL_CreateWindow(ENGINE_NAME, 1920, 1080, SDL_WINDOW_OPENGL);
    if (!m_window)
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

    if (!CreateWindow()) return false;

    if (!AssetsSystem::GetInstance().Init()) return false;
    if (!Input::GetInstance().Init(m_window)) return false;
    if (!Physics::GetInstance().Init()) return false;

    m_renderer = new Renderer();
    if (!m_renderer->Init(m_window, this)) return false;

    m_gui = new Gui();
    if (!m_gui->Init(m_window, this)) return false;
       
    if (!Gizmos::GetInstance().Init()) return false;
    if (!Networking::GetInstance().Init()) return false;
    if (!Entities::GetInstance().Init()) return false;

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
    return SDL_GetPointerProperty(SDL_GetWindowProperties(m_window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
    return nullptr;
#endif
}

void Engine::SetFullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(m_window, fullscreen);
}

void Engine::ToggleFullscreen()
{
    SetFullscreen(!IsInFullscreen());
}

bool Engine::IsInFullscreen()
{
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return (flags & SDL_WINDOW_FULLSCREEN);
}

void Engine::OnWindowResize()
{
    int width, height;
    SDL_GetWindowSizeInPixels(m_window, &width, &height);
    m_renderer->OnWindowResize(width, height);
}
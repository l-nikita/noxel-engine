#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <iostream>
#include <string>
#include <chrono>

#include "engine.hpp"
#include "panel.hpp"
#include "profiler.hpp"
#include "assets_system.hpp"
#include "renderer.hpp"
#include "event_handler.hpp"
#include "input.hpp"
#include "cmd.hpp"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Engine::Engine()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Update()
{
    EventHandler::GetInstance().HandleEvents();

    if(Renderer::GetInstance().m_Camera)
        Renderer::GetInstance().m_Camera->Update();

    bool isGrave = Input::GetInstance().IsKeyPressed(SDL_SCANCODE_GRAVE);
    bool isF3 = Input::GetInstance().IsKeyPressed(SDL_SCANCODE_F3);
    bool isF11 = Input::GetInstance().IsKeyPressed(SDL_SCANCODE_F11);

    if (isGrave)
        m_DebugMenu->m_IsVisible = !m_DebugMenu->m_IsVisible;
    else if (isF3)
        Input::GetInstance().SetRelativeMouseMode(!Input::GetInstance().GetRelativeMouseMode());
    else if (isF11)
        ToggleFullscreen();
}

void Engine::Render()
{
    Renderer::GetInstance().Draw();
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

        swapBuffers.Start();
        SDL_GL_SwapWindow(m_Window);
        swapBuffers.End();
    }

    Stop();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Stop()
{
    m_isRunning = false;

    if(m_Window)
        SDL_DestroyWindow(m_Window);
    if(m_Context)
        SDL_GL_DestroyContext(m_Context);

    SDL_Quit();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::InitGUI()
{
    m_DebugMenu = std::make_unique<DebugMenu>();
    m_panels.push_back(dynamic_cast<Panel*>(m_DebugMenu.get()));

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::CreateWindow()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_Window = SDL_CreateWindow("SDL3 Window", 1920, 1080, SDL_WINDOW_OPENGL);
    if (!m_Window)
    {
        CMD_LOG("SDL_CreateWindow Error: " + std::string(SDL_GetError()), ERROR);
        Stop();
        return false;
    }

    return true;
}

bool Engine::CreateContext()
{
    m_Context = SDL_GL_CreateContext(m_Window);
    if (!m_Context)
    {
        CMD_LOG("SDL_GL_CreateContext Error: " + std::string(SDL_GetError()), ERROR);
        Stop();
        return false;
    }

    return true;
}

bool Engine::LoadGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        CMD_LOG("Failed to initialize GLAD!", ERROR);
        Stop();
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
    if (!CreateContext()) return false;
    if (!LoadGLAD()) return false;
    if (!InitGUI()) return false;

    AssetsSystem::GetInstance().Init();
    Input::GetInstance().Init(m_Window);
    Renderer::GetInstance().Init(m_Window);

    CMD::GetInstance().Setup();
    CMD_LOG("Engine initialized successfully.", INFO);

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::SetFullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(m_Window, fullscreen);
}

void Engine::ToggleFullscreen()
{
    SetFullscreen(!IsInFullscreen());
}

bool Engine::IsInFullscreen()
{
    Uint32 flags = SDL_GetWindowFlags(m_Window);
    return (flags & SDL_WINDOW_FULLSCREEN);
}
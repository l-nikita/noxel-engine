#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>

#include "engine.hpp"
#include "panel.hpp"
#include "profiler.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "cmd.hpp"
#include "assets_system.hpp"

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
    glfwPollEvents();
    if(Renderer::GetInstance().m_Camera)
        Renderer::GetInstance().m_Camera->Update();
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

    while (!glfwWindowShouldClose(m_Window))
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

        swapBuffers.Start();
        glfwSwapBuffers(m_Window);
        swapBuffers.End();
    }

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Stop()
{
    glfwSetFramebufferSizeCallback(m_Window, nullptr);
    glfwDestroyWindow(m_Window);
    glfwTerminate();
    m_Window = nullptr;
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
bool Engine::Init()
{
    CMD_LOG("Hello! I am engine!", INFO);

#ifdef _DEBUG
    CMD_LOG("Built in DEBUG mode.", INFO);
#else
    CMD_LOG("Built in RELEASE mode.", INFO);
#endif

    if (!glfwInit()) 
    {
        CMD_LOG("GLFW init failed", ERROR);
        return false;
    }

    m_Window = glfwCreateWindow(1920, 1080, "GLFW Window", nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        CMD_LOG("Window creation failed", ERROR);
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    if (!Renderer::GetInstance().LoadGLLoader())
    {
        glfwTerminate();
        CMD_LOG("Failed to initialize glad", ERROR);
        return false;
    }

    glfwSetFramebufferSizeCallback(m_Window, Callback_OnFrameBufferSize);

    AssetsSystem::GetInstance().Init();
    Input::GetInstance().Init(m_Window);
    Renderer::GetInstance().Init(m_Window);
    InitGUI();

    CMD::GetInstance().Setup();
    CMD_LOG("Engine initialized successfully.", INFO);

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Callback_OnFrameBufferSize(GLFWwindow* window, int width, int height)
{
    Renderer::GetInstance().OnWindowResize(width, height);
}
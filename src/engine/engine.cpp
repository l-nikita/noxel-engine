#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <chrono>

#include "engine.hpp"
#include "profiler.hpp"

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
}

void Engine::Render()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    if (!m_FirstFrame)
    {
        m_FirstFrame = true;
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    ImGui::NewFrame();

    if (m_DebugMenu)
        m_DebugMenu->Draw();

    ImGui::ShowDemoWindow();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Run()
{
    //using namespace std::chrono;
    //auto lastTime = high_resolution_clock::now();

    Profiler updateProfiler("Update");
    Profiler renderProfiler("Render");

    while (!glfwWindowShouldClose(m_Window))
    {
        //auto currentTime = high_resolution_clock::now();
        //auto deltaTime = duration_cast<duration<float, std::milli>>(currentTime - lastTime);
        //std::cout << "Delta Time: " << deltaTime.count() << " ms" << std::endl;
        //lastTime = currentTime;

        updateProfiler.Start();
        Update();
        updateProfiler.End();

        renderProfiler.Start();
        Render();
        renderProfiler.End();

        glfwSwapBuffers(m_Window);
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
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    m_DebugMenu = std::make_unique<DebugMenu>();

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Engine::Init()
{
    std::cout << "Hello! I am engine!" << std::endl;

    if (!glfwInit()) 
    {
        std::cerr << "GLFW init failed\n";
        return false;
    }

    m_Window = glfwCreateWindow(1920, 1080, "GLFW Window", nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        std::cerr << "Window creation failed\n";
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        glfwTerminate();
        std::cerr << "Failed to initialize glad\n";
        return false;
    }

    glfwSetFramebufferSizeCallback(m_Window, Callback_OnFrameBufferSize);

    InitGUI();

    const char* version = (const char*)glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;
    std::cout << "Engine initialized successfully." << std::endl;

    return true;
}
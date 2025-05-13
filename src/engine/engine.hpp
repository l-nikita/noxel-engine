#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "debug_menu.hpp"
#include "singleton.hpp"

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

class ENGINE_API Engine : public Singleton<Engine>
{
public:
    Engine();

    bool Init();
    bool InitGUI();
    void Run();
    void Stop();

    void Update();
    void Render();

    GLFWwindow* m_Window = nullptr;

private:
    std::unique_ptr<DebugMenu> m_DebugMenu;
    bool m_FirstFrame = false;
};

static void Callback_OnFrameBufferSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
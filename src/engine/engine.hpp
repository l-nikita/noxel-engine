#pragma once

#include "GLFW/glfw3.h"

#include "common.hpp"
#include "debug_menu.hpp"
#include "console_panel.hpp"
#include "singleton.hpp"
#include "panel.hpp"

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

    std::chrono::duration<float, std::milli> DeltaDuration() const { return m_deltaTime; }
    // Delta time (ms)
    float DeltaTime() const { return m_deltaTime.count(); }    
    
    std::chrono::duration<float, std::milli> ElapsedDuration() const { return m_elapsedTime; }
    // Elapsed time since engine loop ran (ms)
    float ElapsedTime() const { return m_elapsedTime.count(); }

    GLFWwindow* m_Window = nullptr;

    std::vector<Panel*> GetPanels() { return m_panels; }
    std::unique_ptr<DebugMenu> m_DebugMenu;

private:
    std::chrono::duration<float, std::milli> m_deltaTime;
    std::chrono::duration<float, std::milli> m_elapsedTime;

    std::vector<Panel*> m_panels;

    static void Callback_OnFrameBufferSize(GLFWwindow* window, int width, int height);
};
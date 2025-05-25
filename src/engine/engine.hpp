#pragma once

#include "SDL3/SDL.h"

#include "common.hpp"
#include "debug_menu.hpp"
#include "console_panel.hpp"
#include "singleton.hpp"
#include "panel.hpp"

class ENGINE_API Engine : public Singleton<Engine>
{
public:
    bool Init();
    bool InitGUI();
    bool CreateWindow();
    bool CreateContext();
    bool LoadGLAD();

    void Run();
    void Stop();

    void Update();
    void Render();

    void SetFullscreen(bool fullscreen);
    void ToggleFullscreen();
    bool IsInFullscreen();

    void OnWindowResize();

    std::chrono::duration<float, std::milli> DeltaDuration() const { return m_deltaTime; }
    // Delta time (ms)
    float DeltaTime() const { return m_deltaTime.count(); }    
    
    std::chrono::duration<float, std::milli> ElapsedDuration() const { return m_elapsedTime; }
    // Elapsed time since engine loop ran (ms)
    float ElapsedTime() const { return m_elapsedTime.count(); }

    SDL_Window* m_Window = nullptr;
    SDL_GLContext m_Context = nullptr;

    std::vector<Panel*> GetPanels() { return m_panels; }
    std::unique_ptr<DebugMenu> m_DebugMenu;

    bool IsRunning() { return m_isRunning; }

private:
    bool m_isRunning;

    std::chrono::duration<float, std::milli> m_deltaTime;
    std::chrono::duration<float, std::milli> m_elapsedTime;

    std::vector<Panel*> m_panels;
};
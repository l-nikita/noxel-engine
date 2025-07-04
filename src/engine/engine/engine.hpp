#pragma once

#include "SDL3/SDL.h"
#include <chrono>

#include "iengine.hpp"
#include "renderer/irenderer.hpp"
#include "common.hpp"

class ENGINE_API Engine : public IEngine
{
public:
    ~Engine() { Shutdown(); }

    bool Init() override;
    bool CreateWindow();

    void Run() override;
    void Stop() override;
    void Shutdown() override;
    bool IsRunning() override { return m_isRunning; }

    void Update() override;
    void Render() override;

    SDL_Window* GetWindow() override { return m_window; }
    int GetWindowWidth() override { return 0; }
    int GetWindowHeight() override { return 0; }
    void* GetNativeWindowHandle() override;
    void OnWindowResize() override;

    void ToggleFullscreen() override;
    void SetFullscreen(bool fullscreen) override;
    bool IsInFullscreen() override;

    // -------------------------------------------------
    std::chrono::duration<float, std::milli> DeltaDuration() const { return m_deltaTime; }
    // Delta time (ms)
    float DeltaTime() override { return m_deltaTime.count(); }
    
    std::chrono::duration<float, std::milli> ElapsedDuration() const { return m_elapsedTime; }
    // Elapsed time since engine loop ran (ms)
    float ElapsedTime() override { return m_elapsedTime.count(); }
    // -------------------------------------------------

    virtual GraphicsAPI GetGraphicsAPI() { return DIRECTX11; }

private:
    SDL_Window* m_window = nullptr;

    bool m_isRunning = false;

    std::chrono::duration<float, std::milli> m_deltaTime;
    std::chrono::duration<float, std::milli> m_elapsedTime;
};
#pragma once

#include <chrono>
#include "common.hpp"
#include "iengine.hpp"
#include "renderer/irenderer.hpp"

class Window;

class ENGINE_API Engine : public IEngine
{
public:
    bool Init() override;
    void InitWindow();

    void Run() override;
    void Stop() override;
    void Shutdown() override;

    bool IsRunning() override
    {
        return m_isRunning;
    }

    void Update() override;
    void Render() override;
    void DrawPlayer() override; // REMOVE

    // Current active window.
    Window* GetWindow() override
    {
        return m_window;
    }

    void OnWindowResize() override;

    //-------------------------------------------------
    std::chrono::duration<float> DeltaDuration() const
    {
        return m_deltaTime;
    }

    // Delta time (ms)
    float DeltaTime() override
    {
        return m_deltaTime.count();
    }

    std::chrono::duration<float> ElapsedDuration() const
    {
        return m_elapsedTime;
    }

    // Elapsed time since engine loop ran (ms)
    float ElapsedTime() override
    {
        return m_elapsedTime.count();
    }

    float GetFPS() override
    {
        return 1.0f / DeltaTime();
    }

    //-------------------------------------------------

    GraphicsAPI GetGraphicsAPI() override
    {
        return m_graphicsAPI;
    }

private:
    Window* m_window = nullptr;
    bool m_isRunning = false;

    std::chrono::duration<float> m_deltaTime{};
    std::chrono::duration<float> m_elapsedTime{};

    GraphicsAPI m_graphicsAPI = GraphicsAPI::NONE;
};

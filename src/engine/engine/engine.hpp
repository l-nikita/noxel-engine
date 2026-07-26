#pragma once

#include <chrono>
#include "common.hpp"
#include "iengine.hpp"
#include "renderer/irenderer.hpp"

class IFileSystem;
class IAssetSystem;
class IInput;
class IPhysics;
class IRenderPipeline;
class IGui;
union SDL_Event;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ENGINE_API Engine : public IEngine
{
public:
    void Initialize() override;

    void Run() override;
    void Quit() override;
    void Shutdown() override;

    bool IsRunning() override;

    void Update() override;
    void FixedUpdate() override;
    void Render() override;
    void DrawPlayer() override; // REMOVE

    //-------------------------------------------------

    // Current active window.
    Window* GetWindow() override;
    void OnWindowResize() override;

    //-------------------------------------------------

    std::chrono::duration<float> DeltaDuration() const;
    float DeltaTime() override;
    float FixedAlpha() override;

    std::chrono::duration<float> ElapsedDuration() const;
    float ElapsedTime() override;

    float GetFPS() override;

    //-------------------------------------------------

    GraphicsAPI GetGraphicsAPI() override;

private:
    void SetupWindow();

    void HandleEvents();
    void OnEvent( const SDL_Event& event );

private:
    void InitializeSystems();
    void ShutdownSystems();

    IFileSystem* m_fileSystem = nullptr;
    IAssetSystem* m_assetSystem = nullptr;
    IInput* m_input = nullptr;
    IPhysics* m_physics = nullptr;
    IRenderer* m_renderer = nullptr;
    IRenderPipeline* m_renderPipeline = nullptr;
    IGui* m_gui = nullptr;

    std::vector<ISystem*> m_systems = {};

public:
    IFileSystem& FileSystem() const override;
    IAssetSystem& AssetSystem() const override;
    IInput& Input() const override;
    IPhysics& Physics() const override;
    IRenderer& Renderer() const override;
    IRenderPipeline& RenderPipeline() const override;
    IGui& Gui() const override;

private:
    Window* m_window = nullptr;
    bool m_isRunning = false;
    float m_accumulator = 0.0f;

    std::chrono::duration<float> m_deltaTime = {};
    std::chrono::duration<float> m_elapsedTime = {};

    GraphicsAPI m_graphicsAPI = GraphicsAPI::NONE;
};

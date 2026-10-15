#pragma once

#include <chrono>

#include "common.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "engine/iengine.hpp"
#include "input/iinput.hpp"
#include "physics/iphysics.hpp"
#include "renderer/irenderpipeline.hpp"
#include "renderer/rendersystem.hpp"

//-----------------------------------------------------------------------------

class IGui;
union SDL_Event;

//-----------------------------------------------------------------------------

using fs::IFileSystem;
using assets::IAssetSystem;
using input::IInput;
using phys::IPhysics;
using render::RenderSystem;
using render::IRenderPipeline;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ENGINE_API Engine final : public IEngine
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

    // Current active window
    Window* GetPrimaryWindow() override;

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
    RenderSystem* m_render = nullptr;
    IRenderPipeline* m_renderPipeline = nullptr;
    // IGui* m_gui = nullptr;

    std::vector<ISystem*> m_systems = {};

private:
    Window* m_window = nullptr;
    std::vector<Window*> m_windows = {};

    bool m_isRunning = false;
    float m_accumulator = 0.0f;

    std::chrono::duration<float> m_deltaTime = {};
    std::chrono::duration<float> m_elapsedTime = {};

    GraphicsAPI m_graphicsAPI = GraphicsAPI::NONE;
};

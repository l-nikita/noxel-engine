#pragma once

#include "common.hpp"
#include "renderer/irenderer.hpp"

class Window;
class IFileSystem;
class IAssetSystem;
class IInput;
class IPhysics;
class IRenderPipeline;
class IGui;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ENGINE_API IEngine
{
public:
    virtual ~IEngine() = default;

    virtual void Initialize() = 0;

    virtual void Run() = 0;
    virtual void Stop() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsRunning() = 0;

    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
    virtual void Render() = 0;
    virtual void DrawPlayer() = 0; // REMOVE

    virtual float DeltaTime() = 0;
    virtual float FixedAlpha() = 0;
    virtual float ElapsedTime() = 0;
    virtual float GetFPS() = 0;

    virtual Window* GetWindow() = 0;
    virtual void OnWindowResize() = 0;

    virtual GraphicsAPI GetGraphicsAPI() = 0;

    virtual IFileSystem& FileSystem() const = 0;
    virtual IAssetSystem& AssetSystem() const = 0;
    virtual IInput& Input() const = 0;
    virtual IPhysics& Physics() const = 0;
    virtual IRenderer& Renderer() const = 0;
    virtual IRenderPipeline& RenderPipeline() const = 0;
    virtual IGui& Gui() const = 0;
};

extern ENGINE_API IEngine* g_Engine;

#pragma once

#include "common.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
enum class GraphicsAPI : int
{
    NONE,
    DIRECTX11,
    VULKAN,
};

//-----------------------------------------------------------------------------

class Window;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ENGINE_API IEngine
{
public:
    virtual ~IEngine() = default;

    virtual void Initialize() = 0;

    virtual void Run() = 0;
    virtual void Quit() = 0;
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

    virtual Window* GetPrimaryWindow() = 0;

    virtual GraphicsAPI GetGraphicsAPI() = 0;
};

extern ENGINE_API IEngine* g_Engine;

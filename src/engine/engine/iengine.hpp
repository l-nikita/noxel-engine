#pragma once

#include "common.hpp"

class IRenderer;
class IGui;
struct SDL_Window; // IWindow

class ENGINE_API IEngine
{
public:
	virtual ~IEngine() = default;

	virtual bool Init() = 0;

	virtual void Run() = 0;
	virtual void Shutdown() = 0;
	virtual bool IsRunning() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual float DeltaTime() = 0;
	virtual float ElapsedTime() = 0;

	virtual SDL_Window* GetWindow() = 0;
	virtual int GetWindowWidth() = 0;
	virtual int GetWindowHeight() = 0;
	virtual void* GetNativeWindowHandle() = 0;
	virtual void OnWindowResize() = 0;

	virtual void ToggleFullscreen() = 0;
	virtual bool IsInFullscreen() = 0;
	virtual void SetFullscreen(bool fullscreen) = 0;
};

extern ENGINE_API IEngine* g_Engine;
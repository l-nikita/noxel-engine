#pragma once

#include "common.hpp"

class IRenderer;
class IGui;
class Window;
enum GraphicsAPI;

class ENGINE_API IEngine
{
public:
	virtual ~IEngine() = default;

	virtual bool Init() = 0;
	virtual bool InitWindow() = 0;

	virtual void Run() = 0;
	virtual void Stop() = 0;
	virtual void Shutdown() = 0;
	virtual bool IsRunning() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void DrawPlayer() = 0; // REMOVE

	virtual float DeltaTime() = 0;
	virtual float ElapsedTime() = 0;
	virtual float GetFPS() = 0;

	virtual Window* GetWindow() = 0;
	virtual void OnWindowResize() = 0;

	virtual GraphicsAPI GetGraphicsAPI() = 0;
};

extern ENGINE_API IEngine* g_Engine;
#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "irenderer.hpp"

class RenderCamera;

class Renderer : public IRenderer
{
public:
	bool Init() override;
	bool InitAPI();

	void Render() override;
	void Shutdown() override;

	void OnWindowResize(int width, int height) override;

	RenderCamera* GetCamera() { return m_camera; };

#ifdef _DEBUG
	int m_DrawCallCount;
	void AddDrawCallCount() override { m_DrawCallCount++; }
	int GetDrawCallCount() override { return m_DrawCallCount; }
#endif

	SDL_GLContext m_Context = nullptr;

private:
	RenderCamera* m_camera;
};
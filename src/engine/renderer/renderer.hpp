#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "common.hpp"
#include "singleton.hpp"
#include "render_camera.hpp"
#include "angles.hpp"

class Renderer : public Singleton<Renderer>
{
public:
	std::unique_ptr<RenderCamera> m_Camera;

	bool Init(SDL_Window* window);
	void InitGUI();
	void InitAPI();

	void Draw();
	void DrawGUI();

	void OnWindowResize(int width, int height);
	void OnWindowClose();

#ifdef _DEBUG
	int m_DrawCallCount;
#endif

private:
	SDL_Window* m_window = nullptr;
	bool m_firstFrame = false;
};
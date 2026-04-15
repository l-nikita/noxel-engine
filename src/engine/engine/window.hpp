#pragma once

#include <string>
#include "SDL3/SDL.h"

class Window
{
public:
	Window(std::string title, int width, int height);
	~Window();

	void Destroy();
	void GetSize(int *width, int *height);

	void ToggleFullscreen();
	void SetFullscreen(bool fullscreen);
	bool IsInFullscreen();

	bool IsMinimized();

	void* GetPointer();

	SDL_WindowFlags GetFlags() { return SDL_GetWindowFlags(m_handle); }
	SDL_Window* GetHandle() { return m_handle; }

private:
	SDL_Window* m_handle = nullptr;
};
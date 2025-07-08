//TODO: Make better

#include <imgui_impl_sdl3.h>

#include "event_handler.hpp"
#include "input/input.hpp"

void EventHandler::HandleEvents()
{
	m_profiler.Start();
	SDL_Event event;

	while (SDL_PollEvent(&event))
		OnEvent(event);
	m_profiler.End();
}

void EventHandler::OnEvent(SDL_Event& event)
{
	ImGui_ImplSDL3_ProcessEvent(&event);

	auto type = event.type;
	switch (type)
	{
		case SDL_EVENT_QUIT:
			g_Engine->Stop();
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			g_Engine->OnWindowResize();
			break;
		// input events
		case SDL_EVENT_KEY_DOWN:
			g_Input->OnKey(event.key);
			break;
		case SDL_EVENT_KEY_UP:
			g_Input->OnKey(event.key);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			g_Input->OnMouseMotion(event.motion);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			g_Input->OnMouseButton(event.button);
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			g_Input->OnMouseButton(event.button);
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			g_Input->OnMouseWheel(event.wheel);
			break;
	}
}
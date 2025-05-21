#include <imgui_impl_sdl3.h>

#include "event_handler.hpp"
#include "input.hpp"

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
			Engine::GetInstance().Stop();
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			Engine::GetInstance().OnWindowResize();
			break;

		// input events
		case SDL_EVENT_KEY_DOWN:
			Input::GetInstance().OnKey(event.key);
			break;
		case SDL_EVENT_KEY_UP:
			Input::GetInstance().OnKey(event.key);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			Input::GetInstance().OnMouseMotion(event.motion);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			Input::GetInstance().OnMouseButton(event.button);
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			Input::GetInstance().OnMouseButton(event.button);
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			Input::GetInstance().OnMouseWheel(event.wheel);
			break;
	}
}
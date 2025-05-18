#pragma once

#include "SDL3/SDL.h"
#include "engine.hpp"
#include "singleton.hpp"

class EventHandler : public Singleton<EventHandler>
{
public:
	EventHandler() 
		: m_profiler("HandleEvents") {}

	void HandleEvents();
	void OnEvent(SDL_Event& event);

private:
	Profiler m_profiler;
};
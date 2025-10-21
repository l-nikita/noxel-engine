#pragma once

#include "SDL3/SDL.h"
#include "common/singleton.hpp"

class EventHandler : public Singleton<EventHandler>
{
public:
	void HandleEvents();
	void OnEvent(SDL_Event& event);
};
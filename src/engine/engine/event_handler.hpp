#pragma once

#include <SDL3/SDL.h>
#include "common/singleton.hpp"

class EventHandler : public Singleton<EventHandler>
{
public:
    static void HandleEvents();
    static void OnEvent( const SDL_Event& event );
};

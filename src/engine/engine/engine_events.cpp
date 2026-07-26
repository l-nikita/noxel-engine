#include <imgui_impl_sdl3.h>

#include "engine.hpp"
#include "dbg/debug.hpp"
#include "input/input.hpp"

void Engine::HandleEvents()
{
    PROF_START( "Engine::HandleEvents", "EventHandler" );

    SDL_Event event;

    while ( SDL_PollEvent( &event ) )
        OnEvent( event );

    PROF_END( "Engine::HandleEvents" );
}

void Engine::OnEvent( const SDL_Event& event )
{
    ImGui_ImplSDL3_ProcessEvent( &event );

    switch ( event.type )
    {
        case SDL_EVENT_QUIT:
            g_Engine->Quit();
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            g_Engine->OnWindowResize();
            break;
        // input event
        case SDL_EVENT_KEY_DOWN:
            g_Input->OnKey( event.key );
            break;
        case SDL_EVENT_KEY_UP:
            g_Input->OnKey( event.key );
            break;
        case SDL_EVENT_MOUSE_MOTION:
            g_Input->OnMouseMotion( event.motion );
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            g_Input->OnMouseButton( event.button );
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            g_Input->OnMouseButton( event.button );
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            g_Input->OnMouseWheel( event.wheel );
            break;
    }
}

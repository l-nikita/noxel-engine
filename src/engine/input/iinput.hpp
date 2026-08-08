#pragma once

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include "common.hpp"

namespace input
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class IInput : public ISystem
    {
    public:
        virtual bool IsKeyDown( SDL_Scancode key ) = 0;
        virtual bool IsKeyPressed( SDL_Scancode key ) = 0;
        virtual bool IsKeyReleased( SDL_Scancode key ) = 0;

        virtual bool IsButtonDown( std::uint8_t button ) = 0;
        virtual bool IsButtonPressed( std::uint8_t button ) = 0;
        virtual bool IsButtonReleased( std::uint8_t button ) = 0;

        virtual void Reset() = 0;

        virtual void SetRelativeMouseMode( bool value ) = 0;
        virtual bool GetRelativeMouseMode() = 0;

        virtual glm::vec2 GetMousePos() = 0;
        virtual glm::vec2 GetMouseDelta() = 0;
        virtual glm::vec2 GetMouseScroll() = 0;

        virtual void OnKey( SDL_KeyboardEvent keyEvent ) = 0;
        virtual void OnMouseMotion( SDL_MouseMotionEvent mouseEvent ) = 0;
        virtual void OnMouseButton( SDL_MouseButtonEvent buttonEvent ) = 0;
        virtual void OnMouseWheel( SDL_MouseWheelEvent wheelEvent ) = 0;
    };
}

extern input::IInput* g_Input;

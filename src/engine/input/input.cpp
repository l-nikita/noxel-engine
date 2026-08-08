#include <SDL3/SDL.h>

#include "input/input.hpp"
#include "engine/engine.hpp"
#include "engine/window.hpp"

namespace input
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SysInit Input::Initialize()
    {
        return {};
    }

    void Input::Shutdown()
    {
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    bool Input::IsKeyDown( const SDL_Scancode key )
    {
        return m_keyDown[ key ];
    }

    bool Input::IsKeyPressed( const SDL_Scancode key )
    {
        return m_keyPressed[ key ];
    }

    bool Input::IsKeyReleased( const SDL_Scancode key )
    {
        return m_keyReleased[ key ];
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    bool Input::IsButtonDown( const Uint8 button )
    {
        return m_mouseDown[ button ];
    }

    bool Input::IsButtonPressed( const Uint8 button )
    {
        return m_mousePressed[ button ];
    }

    bool Input::IsButtonReleased( const Uint8 button )
    {
        return m_mouseReleased[ button ];
    }

    void Input::Reset()
    {
        for ( auto i = 0; i < SDL_SCANCODE_COUNT; ++i )
        {
            m_keyPressed[ static_cast<SDL_Scancode>(i) ] = false;
            m_keyReleased[ static_cast<SDL_Scancode>(i) ] = false;
        }

        for ( auto i = 0; i < 8; ++i )
        {
            m_mousePressed[ i ] = false;
            m_mouseReleased[ i ] = false;
        }

        m_scrollDelta = glm::vec2( 0.0f );
    }

    void Input::SetRelativeMouseMode( const bool value )
    {
        SDL_SetWindowRelativeMouseMode( g_Engine->GetPrimaryWindow()->GetHandle(), value );
    }

    bool Input::GetRelativeMouseMode()
    {
        return SDL_GetWindowRelativeMouseMode( g_Engine->GetPrimaryWindow()->GetHandle() );
    }

    glm::vec2 Input::GetMousePos()
    {
        float x, y;
        SDL_GetMouseState( &x, &y );
        return {x, y};
    }

    glm::vec2 Input::GetMouseDelta()
    {
        float dx, dy;
        SDL_GetRelativeMouseState( &dx, &dy );
        return {dx, dy};
    }

    glm::vec2 Input::GetMouseScroll()
    {
        return m_scrollDelta;
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void Input::OnKey( const SDL_KeyboardEvent keyEvent )
    {
        if ( keyEvent.type == SDL_EVENT_KEY_DOWN && !keyEvent.repeat )
        {
            m_keyDown[ keyEvent.scancode ] = true;
            m_keyPressed[ keyEvent.scancode ] = true;
            if ( m_keyReleased[ keyEvent.scancode ] )
                m_keyReleased[ keyEvent.scancode ] = false;
        } else if ( keyEvent.type == SDL_EVENT_KEY_UP )
        {
            m_keyDown[ keyEvent.scancode ] = false;
            if ( !m_keyReleased[ keyEvent.scancode ] )
                m_keyReleased[ keyEvent.scancode ] = true;
        }
    }

    void Input::OnMouseMotion( SDL_MouseMotionEvent mouseEvent )
    {
    }

    void Input::OnMouseButton( const SDL_MouseButtonEvent buttonEvent )
    {
        if ( buttonEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN )
        {
            m_mouseDown[ buttonEvent.button ] = true;
            m_mousePressed[ buttonEvent.button ] = true;
            if ( m_mouseReleased[ buttonEvent.button ] )
                m_mouseReleased[ buttonEvent.button ] = false;
        } else if ( buttonEvent.type == SDL_EVENT_MOUSE_BUTTON_UP )
        {
            m_mouseDown[ buttonEvent.button ] = false;
            if ( !m_mouseReleased[ buttonEvent.button ] )
                m_mouseReleased[ buttonEvent.button ] = true;
        }
    }

    void Input::OnMouseWheel( const SDL_MouseWheelEvent wheelEvent )
    {
        m_scrollDelta += glm::vec2( wheelEvent.x, wheelEvent.y );
    }
}

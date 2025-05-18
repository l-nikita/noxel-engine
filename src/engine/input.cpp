#include "engine.hpp"
#include "input.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Input::Init(SDL_Window* window)
{
    m_window = window;

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Input::IsKeyDown(SDL_Scancode key)
{
    return m_keyDown[key];
}

bool Input::IsKeyPressed(SDL_Scancode key)
{
    if (m_keyPressed[key]) 
    {
        m_keyPressed[key] = false;
        return true;
    }

    return false;
}

bool Input::IsKeyReleased(SDL_Scancode key)
{
    if (m_keyReleased[key])
    {
        m_keyReleased[key] = false;
        return true;
    }

    return m_keyReleased[key];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Input::IsButtonDown(Uint8 button)
{
    return m_mouseDown[button];
}

bool Input::IsButtonPressed(Uint8 button)
{
    if (m_mousePressed[button])
    {
        m_mousePressed[button] = false;
        return true;
    }

    return false;
}

bool Input::IsButtonReleased(Uint8 button)
{
    if (m_mouseReleased[button])
    {
        m_mouseReleased[button] = false;
        return true;
    }

    return m_mouseReleased[button];
}

void Input::SetRelativeMouseMode(bool value)
{
    SDL_SetWindowRelativeMouseMode(m_window, value);
}

bool Input::GetRelativeMouseMode()
{
    return SDL_GetWindowRelativeMouseMode(m_window);
}

glm::vec2 Input::GetMousePos()
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    return glm::vec2(x, y);
}

glm::vec2 Input::GetMouseDelta()
{
    float dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);
    return glm::vec2(dx, dy);
}

glm::vec2 Input::GetMouseScroll() const
{
    return m_scrollDelta;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Input::OnKey(SDL_KeyboardEvent keyEvent)
{
    if (keyEvent.type == SDL_EVENT_KEY_DOWN && !keyEvent.repeat)
    {
        m_keyDown[keyEvent.scancode] = true;
        m_keyPressed[keyEvent.scancode] = true;
        if (m_keyReleased[keyEvent.scancode])
            m_keyReleased[keyEvent.scancode] = false;
    }
    else if (keyEvent.type == SDL_EVENT_KEY_UP)
    {
        m_keyDown[keyEvent.scancode] = false;
        if(!m_keyReleased[keyEvent.scancode])
            m_keyReleased[keyEvent.scancode] = true;
    }
}

void Input::OnMouseMotion(SDL_MouseMotionEvent mouseEvent)
{

}

void Input::OnMouseButton(SDL_MouseButtonEvent buttonEvent)
{
    if (buttonEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        m_mouseDown[buttonEvent.button] = true;
        m_mousePressed[buttonEvent.button] = true;
        if (m_mouseReleased[buttonEvent.button])
            m_mouseReleased[buttonEvent.button] = false;
    }
    else if (buttonEvent.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        m_mouseDown[buttonEvent.button] = false;
        if (!m_mouseReleased[buttonEvent.button])
            m_mouseReleased[buttonEvent.button] = true;
    }
}

void Input::OnMouseWheel(SDL_MouseWheelEvent wheelEvent)
{
    m_scrollDelta += glm::vec2(wheelEvent.x, wheelEvent.y);
}
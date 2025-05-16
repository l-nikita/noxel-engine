#include "engine.hpp"
#include "input.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Input::Init(GLFWwindow* window)
{
    m_window = window;
    glfwSetKeyCallback(m_window, Callback_Key);
    glfwSetCharModsCallback(m_window, Callback_CharMods);

    glfwSetCursorPosCallback(m_window, Callback_MouseMove);
    glfwSetMouseButtonCallback(m_window, Callback_MouseButton);
    glfwSetScrollCallback(m_window, Callback_Scroll);

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Input::IsKeyPressed(const int key)
{
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Input::IsKeyReleased(const int key)
{
    return glfwGetKey(m_window, key) == GLFW_RELEASE;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Input::IsButtonPressed(const int button)
{
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

bool Input::IsButtonReleased(const int button)
{
    return glfwGetMouseButton(m_window, button) == GLFW_RELEASE;
}

glm::vec2 Input::GetMousePos()
{
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return glm::vec2(x, y);
}

glm::vec2 Input::GetMouseDelta()
{
    auto mousePos = GetMousePos();
    glm::vec2 delta = mousePos - m_lastMousePos;
    m_lastMousePos = mousePos;
    return delta;
}

glm::vec2 Input::GetMouseScroll() const
{
    return m_scrollDelta;
}

//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------
void Input::Callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void Input::Callback_CharMods(GLFWwindow* window, unsigned int codepoint, int mods)
{
}

void Input::Callback_MouseMove(GLFWwindow* window, double xpos, double ypos)
{
}

void Input::Callback_MouseButton(GLFWwindow* window, int button, int action, int mods)
{
}

void Input::Callback_Scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    Input::GetInstance().m_scrollDelta += glm::vec2(xoffset, yoffset);
}
#include "window.hpp"
#include "engine.hpp"
#include "dbg/debug.hpp"

Window::Window(std::string title, int width, int height)
{
    if (g_Engine->GetGraphicsAPI() == DIRECTX11)
        m_handle = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
    else if (g_Engine->GetGraphicsAPI() == OPENGL)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        m_handle = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    }
    else
        FATAL_ERROR("Unknown graphics API!", CMD::ENGINE);

    if (!m_handle)
        FATAL_ERROR("SDL_CreateWindow Error: " + std::string(SDL_GetError()), CMD::ENGINE);
}

void Window::Destroy()
{
    SDL_DestroyWindow(m_handle);
}

void Window::GetSize(int* width, int* height)
{
    SDL_GetWindowSizeInPixels(m_handle, width, height);
}

void Window::SetFullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(m_handle, fullscreen);
}

bool Window::IsInFullscreen()
{
    return (GetFlags() & SDL_WINDOW_FULLSCREEN);
}

void Window::ToggleFullscreen()
{
    SetFullscreen(!IsInFullscreen());
}

void* Window::GetPointer()
{
    return SDL_GetPointerProperty(SDL_GetWindowProperties(m_handle), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
}
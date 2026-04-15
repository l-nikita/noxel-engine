#include "window.hpp"
#include "engine.hpp"
#include "dbg/debug.hpp"

Window::Window(std::string title, int width, int height)
{
    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;

    if (g_Engine->GetGraphicsAPI() == OPENGL)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        flags |= SDL_WINDOW_OPENGL;
    }
        
    m_handle = SDL_CreateWindow(title.c_str(), width, height, flags);

    if (!m_handle)
        FATAL_ERROR("SDL_CreateWindow Error: " + std::string(SDL_GetError()), CMD::ENGINE);
}

Window::~Window()
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

bool Window::IsMinimized()
{
    return (GetFlags() & SDL_WINDOW_MINIMIZED);
}

void Window::ToggleFullscreen()
{
    SetFullscreen(!IsInFullscreen());
}

void* Window::GetPointer()
{
    void* ptr = SDL_GetPointerProperty(SDL_GetWindowProperties(m_handle), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    if (!ptr)
        FATAL_ERROR("Couldn't get window pointer from SDL window!", CMD::ENGINE);

    return ptr;
}
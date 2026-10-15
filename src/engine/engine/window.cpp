#include <cassert>

#include "window.hpp"
#include "dbg/debug.hpp"

Window::Window( const std::string& title, const int width, const int height )
{
    auto flags = SDL_WINDOW_RESIZABLE;
    m_handle = SDL_CreateWindow( title.c_str(), width, height, flags );

    if ( !m_handle )
        FATAL_ERROR( "SDL_CreateWindow Error: " + std::string(SDL_GetError()), cmd::ENGINE );
}

Window::~Window()
{
    m_resizeCallbacks.clear();
    SDL_DestroyWindow( m_handle );
}

void Window::OnResize()
{
    auto size = GetSize();
    for ( const auto& callback: m_resizeCallbacks )
        callback( size );
}

WindowSize Window::GetSize() const
{
    int width, height = 0;
    SDL_GetWindowSizeInPixels( m_handle, &width, &height );

    return {width, height};
}

void Window::ToggleFullscreen()
{
    SetFullscreen( !IsInFullscreen() );
}

void Window::SetFullscreen( const bool fullscreen )
{
    SDL_SetWindowFullscreen( m_handle, fullscreen );
}

bool Window::IsInFullscreen() const
{
    return ( GetFlags() & SDL_WINDOW_FULLSCREEN );
}

bool Window::IsMinimized() const
{
    return ( GetFlags() & SDL_WINDOW_MINIMIZED );
}

bool Window::IsInputFocused() const
{
    return ( GetFlags() & SDL_WINDOW_INPUT_FOCUS );
}

#if PLATFORM_WIN32

void* Window::GetHWNDPointer() const
{
    const auto ptr = SDL_GetPointerProperty(
        SDL_GetWindowProperties( m_handle ),
        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
        nullptr );

    if ( !ptr )
        FATAL_ERROR( "Couldn't get window pointer from SDL window!", cmd::ENGINE );

    return ptr;
}

#elif PLATFORM_LINUX

void* Window::GetWlSurface() const
{
    const auto ptr = SDL_GetPointerProperty(
        SDL_GetWindowProperties( m_handle ),
        SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER,
        nullptr );

    if ( !ptr )
        FATAL_ERROR( "Couldn't get Wayland window surface pointer from SDL window!", cmd::ENGINE );

    return ptr;
}

void* Window::GetWlDisplay() const
{
    const auto ptr = SDL_GetPointerProperty(
        SDL_GetWindowProperties( m_handle ),
        SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER,
        nullptr );

    if ( !ptr )
        FATAL_ERROR( "Couldn't get Wayland window display pointer from SDL window!", cmd::ENGINE );

    return ptr;
}

#endif

SDL_WindowFlags Window::GetFlags() const
{
    return SDL_GetWindowFlags( m_handle );
}

SDL_Window* Window::GetHandle() const
{
    return m_handle;
}

SDL_WindowID Window::GetId() const
{
    return SDL_GetWindowID( m_handle );
}

void Window::SubscribeResize( const WindowResizeCallback& callback )
{
    m_resizeCallbacks.push_back( callback );
}

#include "window.hpp"
#include "engine.hpp"
#include "dbg/debug.hpp"

Window::Window( const std::string& title, const int width, const int height )
{
    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;

    if ( g_Engine->GetGraphicsAPI() == GraphicsAPI::OPENGL )
    {
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        flags |= SDL_WINDOW_OPENGL;
    }

    m_handle = SDL_CreateWindow( title.c_str(), width, height, flags );

    if ( !m_handle )
        FATAL_ERROR( "SDL_CreateWindow Error: " + std::string(SDL_GetError()), CMD::ENGINE );
}

Window::~Window()
{
    SDL_DestroyWindow( m_handle );
}

std::tuple<int, int> Window::GetSize() const
{
    int width, height = 0;
    SDL_GetWindowSizeInPixels( m_handle, &width, &height );

    return {width, height};
}

void Window::ToggleFullscreen()
{
    SetFullscreen( !IsInFullscreen() );
}

void Window::SetFullscreen( const bool fullscreen ) const
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

void* Window::GetPointer() const
{
    const auto ptr = SDL_GetPointerProperty( SDL_GetWindowProperties( m_handle ), SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                             nullptr );
    if ( !ptr )
        FATAL_ERROR( "Couldn't get window pointer from SDL window!", CMD::ENGINE );

    return ptr;
}

SDL_WindowFlags Window::GetFlags() const
{
    return SDL_GetWindowFlags( m_handle );
}

SDL_Window* Window::GetHandle() const
{
    return m_handle;
}

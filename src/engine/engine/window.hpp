#pragma once

#include <string>
#include "SDL3/SDL.h"

class Window
{
public:
    Window( const std::string& title, int width, int height );
    ~Window();

    void Destroy();
    void GetSize( int* width, int* height ) const;

    void ToggleFullscreen();
    void SetFullscreen( bool fullscreen ) const;
    bool IsInFullscreen();

    bool IsMinimized();

    void* GetPointer() const;

    SDL_WindowFlags GetFlags() const
    {
        return SDL_GetWindowFlags(m_handle);
    }

    SDL_Window* GetHandle() const
    {
        return m_handle;
    }

private:
    SDL_Window* m_handle = nullptr;
};

#pragma once

#include <string>
#include "SDL3/SDL.h"

class Window
{
public:
    Window( const std::string& title, int width, int height );
    ~Window();

    void GetSize( int* width, int* height ) const;

    void ToggleFullscreen();
    void SetFullscreen( bool fullscreen ) const;
    bool IsInFullscreen() const;
    bool IsMinimized() const;

    void* GetPointer() const;

    SDL_WindowFlags GetFlags() const;
    SDL_Window* GetHandle() const;

private:
    SDL_Window* m_handle = nullptr;
};

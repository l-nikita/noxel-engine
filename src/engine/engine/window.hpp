#pragma once

#include <string>
#include <tuple>
#include "SDL3/SDL.h"

class Window
{
public:
    Window( const std::string& title, int width, int height );
    ~Window();

    std::tuple<int, int> GetSize() const;

    void ToggleFullscreen();
    void SetFullscreen( bool fullscreen ) const;
    bool IsInFullscreen() const;
    bool IsMinimized() const;
    bool IsInputFocused() const;

    void* GetPointer() const;

    SDL_WindowFlags GetFlags() const;
    SDL_Window* GetHandle() const;

private:
    SDL_Window* m_handle = nullptr;
};

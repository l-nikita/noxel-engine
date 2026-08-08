#pragma once

#include <functional>
#include <string>
#include <tuple>
#include <vector>
#include <SDL3/SDL.h>

//-----------------------------------------------------------------------------

using WindowSize = std::tuple<std::uint32_t, std::uint32_t>;
using WindowResizeCallback = std::function<void( const WindowSize& )>;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Window
{
public:
    Window( const std::string& title, int width, int height );
    ~Window();

    void OnResize();
    WindowSize GetSize() const;

    void ToggleFullscreen();
    void SetFullscreen( bool fullscreen );
    bool IsInFullscreen() const;

    bool IsMinimized() const;
    bool IsInputFocused() const;

    void* GetPointer() const;

    SDL_WindowFlags GetFlags() const;
    SDL_Window* GetHandle() const;
    SDL_WindowID GetId() const;

    void SubscribeResize( const WindowResizeCallback& callback );

private:
    SDL_Window* m_handle = nullptr;

    std::vector<WindowResizeCallback> m_resizeCallbacks;
};

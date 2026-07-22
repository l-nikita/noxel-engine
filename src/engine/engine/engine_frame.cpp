#include "engine.hpp"
#include "player.hpp"
#include "window.hpp"
#include "dbg/debug.hpp"
#include "gui/gui.hpp"
#include "gui/igui.hpp"
#include "gui/imgui/debug_menu/debug_menu.hpp"
#include "input/iinput.hpp"
#include "physics/iphysics.hpp"
#include "renderer/irenderpipeline.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Run()
{
    using namespace std::chrono;
    auto lastTime = high_resolution_clock::now();
    const auto startTime = high_resolution_clock::now();

    m_isRunning = true;

    while ( m_isRunning )
    {
        auto currentTime = high_resolution_clock::now();
        m_deltaTime = duration<float>( currentTime - lastTime );
        lastTime = currentTime;
        m_elapsedTime = duration<float>( currentTime - startTime );

        PROF_START( "Engine::Update", "Engine" );
        Update();
        PROF_END( "Engine::Update" );

        PROF_START( "Engine::Render", "Engine" );
        Render();
        PROF_END( "Engine::Render" );
    }

    Shutdown();
}

void Engine::Stop()
{
    m_isRunning = false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Engine::Update()
{
    HandleEvents();

    if ( g_Player )
        g_Player->Update();

    g_Physics->Update();

    const bool isGrave = g_Input->IsKeyPressed( SDL_SCANCODE_GRAVE );
    const bool isF3 = g_Input->IsKeyPressed( SDL_SCANCODE_F3 );
    const bool isF11 = g_Input->IsKeyPressed( SDL_SCANCODE_F11 );

    const auto gui = dynamic_cast<::Gui*>(m_gui);
    if ( isGrave )
        gui->m_DebugMenu->m_IsVisible = !gui->m_DebugMenu->m_IsVisible;
    if ( isF3 )
        g_Input->SetRelativeMouseMode( !g_Input->GetRelativeMouseMode() );
    else if ( isF11 )
        GetWindow()->ToggleFullscreen();

    g_Input->Reset();
}

void Engine::Render()
{
    if ( GetWindow()->IsMinimized() )
        return;

    if ( g_RenderPipeline )
        g_RenderPipeline->Render();
}

void Engine::DrawPlayer()
{
    g_Player->Draw();
}

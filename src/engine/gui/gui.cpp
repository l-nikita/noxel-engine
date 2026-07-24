#include <imgui.h>
#include <imgui_impl_sdl3.h>
#ifdef DX11
#include <imgui_impl_dx11.h>
#else
//#include <imgui_impl_vulkan.h>
#endif

#include "iengine.hpp"
#include "renderer/irenderer.hpp"
#include "window.hpp"
#ifdef DX11
#include "renderer/directx11/dx11renderer.hpp"
#endif
#include "gui.hpp"
#include "panel.hpp"
#include "dbg/debug.hpp"
#include "dbg/cmd/cmd.hpp"
#include "imgui/debug_menu/debug_menu.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SysInit Gui::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().FontGlobalScale = 1.5f;

    auto api = g_Engine->GetGraphicsAPI();
    if ( api == GraphicsAPI::VULKAN )
    {
    } else if ( api == GraphicsAPI::DIRECTX11 )
    {
#ifdef DX11
        const auto dx11render = static_cast<DX11Renderer*>(g_Renderer);
        ImGui_ImplSDL3_InitForD3D( g_Engine->GetWindow()->GetHandle() );
        ImGui_ImplDX11_Init( dx11render->GetDevice(), dx11render->GetContext() );
#endif
    }

    m_DebugMenu = new DebugMenu();
    m_DebugMenu->m_IsVisible = false;
    m_panels.push_back( m_DebugMenu );

    return {};
}

void Gui::Shutdown()
{
    if ( m_DebugMenu )
    {
        delete m_DebugMenu;
        m_DebugMenu = nullptr;
    }

    m_panels.clear();

    const auto api = g_Engine->GetGraphicsAPI();

    if ( api == GraphicsAPI::VULKAN )
    {
        //ImGui_ImplVulkan_Shutdown();
    } else if ( api == GraphicsAPI::DIRECTX11 )
    {
#ifdef DX11
        ImGui_ImplDX11_Shutdown();
#endif
    }

    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Gui::Render()
{
    auto api = g_Engine->GetGraphicsAPI();
    if ( api == GraphicsAPI::VULKAN )
    {
        //ImGui_ImplVulkan_NewFrame();
    } else if ( api == GraphicsAPI::DIRECTX11 )
    {
#ifdef DX11
        ImGui_ImplDX11_NewFrame();
#endif
    }

    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    for ( const auto panel: GetPanels() )
    {
        if ( panel && panel->m_IsVisible )
            panel->Draw();
    }

    ImGui::Render();

    if ( api == GraphicsAPI::VULKAN )
    {
        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
    } else if ( api == GraphicsAPI::DIRECTX11 )
    {
#ifdef DX11
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
#endif
    }
}

//-----------------------------------------------------------------------------
void Gui::Update()
{
}

std::vector<Panel*>& Gui::GetPanels()
{
    return m_panels;
}

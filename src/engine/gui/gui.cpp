#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_dx11.h>

#include "iengine.hpp"
#include "renderer/directx11/dx11renderer.hpp"
#include "gui.hpp"
#include "panel.hpp"
#include "dbg/cmd/cmd.hpp"
#include "imgui/debug_menu/debug_menu.hpp"

bool Gui::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (g_Engine->GetGraphicsAPI() == DIRECTX11)
    {
        auto dx11render = static_cast<DX11Renderer*>(g_Renderer);
        ImGui_ImplSDL3_InitForD3D(g_Engine->GetWindow());
        ImGui_ImplDX11_Init(dx11render->GetDevice(), dx11render->GetContext());
    }
    else
    {
        //auto renderer = static_cast<GLRenderer*>(g_Renderer);
        //ImGui_ImplSDL3_InitForOpenGL(g_Engine->GetWindow(), renderer->GetContext());
        //ImGui_ImplOpenGL3_Init("#version 330");
        assert(false);
    }

    m_DebugMenu = new DebugMenu();
    m_panels.push_back(dynamic_cast<Panel*>(m_DebugMenu));

    return true;
}

void Gui::Render()
{
    auto isDX11 = g_Engine->GetGraphicsAPI() == DIRECTX11;
    if (isDX11)
        ImGui_ImplDX11_NewFrame();

    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    for (auto panel : GetPanels())
    {
        if (panel && panel->m_IsVisible)
            panel->Draw();
    }

    ImGui::Render();

    if(isDX11)
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
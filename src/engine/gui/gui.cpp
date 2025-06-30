#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "iengine.hpp"
#include "renderer/renderer.hpp"
#include "gui.hpp"
#include "panel.hpp"
#include "cmd/cmd.hpp"
#include "imgui/debug_menu/debug_menu.hpp"

bool Gui::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    auto renderer = static_cast<Renderer*>(g_Engine->GetRenderer());
    ImGui_ImplSDL3_InitForOpenGL(g_Engine->GetWindow(), renderer->m_Context);
    ImGui_ImplOpenGL3_Init("#version 330");

    m_DebugMenu = new DebugMenu();
    m_panels.push_back(dynamic_cast<Panel*>(m_DebugMenu));

    return true;
}

void Gui::Render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    for (auto panel : GetPanels())
    {
        if (panel && panel->m_IsVisible)
            panel->Draw();
    }

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Shutdown()
{

}
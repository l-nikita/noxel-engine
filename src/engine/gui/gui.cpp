//#include <imgui.h>
//#include <imgui_impl_sdl3.h>
#ifdef DX11
    //#include <imgui_impl_dx11.h>
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
#include "dbg/cmd/cmd.hpp"
//#include "imgui/debug_menu/debug_menu.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Gui::~Gui()
{
    /*
    if (m_DebugMenu)
    {
        delete m_DebugMenu;
        m_DebugMenu = nullptr;
    }

    m_panels.clear();

    auto api = g_Engine->GetGraphicsAPI();
    if (api == VULKAN)
    {
        //ImGui_ImplVulkan_Shutdown();
    }
    else if (api == DIRECTX11)
    {
#ifdef DX11
        ImGui_ImplDX11_Shutdown();
#endif
    }

    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    */
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Gui::Init()
{
    /*
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto api = g_Engine->GetGraphicsAPI();
    if (api == VULKAN)
    {

    }
    else if (api == DIRECTX11)
    {
#ifdef DX11
        auto dx11render = static_cast<DX11Renderer*>(g_Renderer);
        ImGui_ImplSDL3_InitForD3D(g_Engine->GetWindow()->GetHandle());
        ImGui_ImplDX11_Init(dx11render->GetDevice(), dx11render->GetContext());
#endif
    }
    else if (api == OPENGL)
    {
        //auto renderer = static_cast<GLRenderer*>(g_Renderer);
        //ImGui_ImplSDL3_InitForOpenGL(g_Engine->GetWindow(), renderer->GetContext());
        //ImGui_ImplOpenGL3_Init("#version 330");
        assert(false);
    }

    m_DebugMenu = new DebugMenu();
    m_DebugMenu->m_IsVisible = false;
    m_panels.push_back(dynamic_cast<Panel*>(m_DebugMenu));
    */
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Gui::Render()
{
    /*
    auto api = g_Engine->GetGraphicsAPI();
    if (api == VULKAN)
    {
        //ImGui_ImplVulkan_NewFrame();
    }
    else if (api == DIRECTX11)
    {
#ifdef DX11
        ImGui_ImplDX11_NewFrame();
#endif
    }

    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    for (auto panel : GetPanels())
    {
        if (panel && panel->m_IsVisible)
            panel->Draw();
    }

    ImGui::Render();

    if (api == VULKAN)
    {
        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
    }
    else if (api == DIRECTX11)
    {
#ifdef DX11
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    }
    */
}

//-----------------------------------------------------------------------------
void Gui::Update()
{
    
}
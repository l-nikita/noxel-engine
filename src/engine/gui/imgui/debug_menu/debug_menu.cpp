#include <imgui.h>

#include "engine.hpp"
#include "window.hpp"
#include "debug_menu.hpp"
#include "dbg/cmd/cmd.hpp"
#include "networking/networking.hpp"
#include "client/client.hpp"

DebugMenu::DebugMenu()
{
	m_BudgetPanel = new BudgetPanel();
	m_ConsolePanel = new ConsolePanel();
	m_DevPanel = new DevPanel();
}

DebugMenu::~DebugMenu()
{
	if (m_BudgetPanel)
	{
		delete m_BudgetPanel;
		m_BudgetPanel = nullptr;
	}

	if (m_ConsolePanel)
	{
		delete m_ConsolePanel;
		m_ConsolePanel = nullptr;
	}

	if (m_DevPanel)
	{
		delete m_DevPanel;
		m_DevPanel = nullptr;
	}
}

void DebugMenu::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Engine"))
		{
			if (ImGui::MenuItem("Toggle Fullscreen"))
				g_Engine->GetWindow()->ToggleFullscreen();

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				g_Engine->Stop();

            ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Networking"))
		{
			if (ImGui::MenuItem("Start", NULL, false, !Networking::GetInstance().IsRunning()))
				CMD::Execute("net_start");

			if (ImGui::MenuItem("Connect to local", NULL, false, !Client::GetInstance().IsConnected() && Networking::GetInstance().IsRunning()))
				CMD::Execute("net_connect_local");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::Checkbox("Console", &m_ConsoleEnabled);
			ImGui::Checkbox("Budget", &m_BudgetEnabled);
			ImGui::Checkbox("DevMenu", &m_DevPanelEnabled);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_BudgetEnabled)
		m_BudgetPanel->Draw();	
	
	if (m_ConsoleEnabled)
		m_ConsolePanel->Draw();	
	
	if (m_DevPanelEnabled)
		m_DevPanel->Draw();
}
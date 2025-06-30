#include <imgui.h>

#include "engine.hpp"
#include "debug_menu.hpp"
#include "cmd/cmd.hpp"
#include "networking/networking.hpp"
#include "client/client.hpp"

DebugMenu::DebugMenu()
{
	m_BudgetPanel = std::make_unique<BudgetPanel>();
	m_ConsolePanel = std::make_unique<ConsolePanel>();
	m_DevPanel = std::make_unique<DevPanel>();
}

void DebugMenu::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Engine"))
		{
			if (ImGui::MenuItem("Toggle Fullscreen"))
			{
				//Engine::GetInstance().ToggleFullscreen();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				g_Engine->Shutdown();

            ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Networking"))
		{
			if (ImGui::MenuItem("Start", NULL, false, !Networking::GetInstance().IsRunning()))
				CMD::GetInstance().Execute("net_start");

			if (ImGui::MenuItem("Connect to local", NULL, false, !Client::GetInstance().IsConnected() && Networking::GetInstance().IsRunning()))
				CMD::GetInstance().Execute("net_connect_local");

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
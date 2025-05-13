#include <imgui.h>
#include "debug_menu.hpp"

DebugMenu::DebugMenu()
{
	m_BudgetPanel = std::make_unique<BudgetPanel>();
}

void DebugMenu::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Engine"))
		{
			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				exit(0);

            ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			//ImGui::Checkbox("Console", ref _consoleEnabled);
			ImGui::Checkbox("Budget", &m_BudgetEnabled);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_BudgetEnabled && m_BudgetPanel)
		m_BudgetPanel->Draw();
}
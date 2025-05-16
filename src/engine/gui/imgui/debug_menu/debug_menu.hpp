#pragma once

#include <memory>

#include "panel.hpp"
#include "budget_panel.hpp"
#include "console_panel.hpp"

class DebugMenu : public Panel
{
public:
	DebugMenu();

	std::unique_ptr<BudgetPanel> m_BudgetPanel;
	bool m_BudgetEnabled;	
	
	std::unique_ptr<ConsolePanel> m_ConsolePanel;
	bool m_ConsoleEnabled;

	void Draw();
};
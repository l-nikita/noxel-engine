#pragma once

#include <memory>

#include "panel.hpp"
#include "budget_panel.hpp"

class DebugMenu : public Panel
{
public:
	DebugMenu();

	std::unique_ptr<BudgetPanel> m_BudgetPanel;
	bool m_BudgetEnabled;

	void Draw();
};
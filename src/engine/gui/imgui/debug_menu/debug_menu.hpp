#pragma once

#include "budget_panel.hpp"
#include "console_panel.hpp"
#include "dev_menu.hpp"
#include "gui/panel.hpp"

class DebugMenu : public Panel
{
public:
    DebugMenu();
    ~DebugMenu() override;

    BudgetPanel* m_BudgetPanel;
    bool m_BudgetEnabled;

    ConsolePanel* m_ConsolePanel;
    bool m_ConsoleEnabled;

    DevPanel* m_DevPanel;
    bool m_DevPanelEnabled;

    void Draw() override;
};

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

    BudgetPanel* m_BudgetPanel = nullptr;
    bool m_BudgetEnabled = true;

    ConsolePanel* m_ConsolePanel = nullptr;
    bool m_ConsoleEnabled = true;

    DevPanel* m_DevPanel = nullptr;
    bool m_DevPanelEnabled = true;

    void Draw() override;
};

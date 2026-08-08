#pragma once

#include "igui.hpp"

class DebugMenu;

class Gui : public IGui
{
public:
    SYSTEM_METADATA( GUI, "Gui", SysId::RENDER );

public:
    SysInit Initialize() override;
    void Shutdown() override;
    void Render() override;
    void Update() override;

    std::vector<Panel*>& GetPanels();

    DebugMenu* m_DebugMenu;

private:
    std::vector<Panel*> m_panels;
};

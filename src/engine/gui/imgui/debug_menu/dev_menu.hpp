#pragma once

#include "gui/panel.hpp"

class DevPanel : public Panel
{
public:
    DevPanel();
    void Draw() override;

private:
    bool m_demoViewEnabled = false;
    bool m_vsyncEnabled;
};

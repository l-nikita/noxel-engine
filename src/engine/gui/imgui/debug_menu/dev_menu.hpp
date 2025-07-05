#pragma once

#include "gui/panel.hpp"

class DevPanel : public Panel
{
public:
	DevPanel() { m_vsyncEnabled = g_Renderer->IsVSyncEnabled(); }
	void Draw();

private:
	bool m_demoViewEnabled = false;
	bool m_vsyncEnabled;
};
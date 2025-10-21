#pragma once

#include "gui/panel.hpp"

class DevPanel : public Panel
{
public:
	DevPanel();
	void Draw();

private:
	bool m_demoViewEnabled = false;
	bool m_vsyncEnabled;
};
#pragma once

#include "panel.hpp"

class DevPanel : public Panel
{
public:
	void Draw();

private:
	bool m_demoViewEnabled;
};
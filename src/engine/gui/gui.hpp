#pragma once

#include "igui.hpp"

class DebugMenu;

class Gui : public IGui
{
public:
	void Init() override;
	void Render() override;
	void Shutdown() override;

	std::vector<Panel*>& GetPanels() override { return m_panels; }
	DebugMenu* m_DebugMenu;

private:
	std::vector<Panel*> m_panels;
};
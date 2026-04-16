#pragma once

#include "igui.hpp"

class DebugMenu;

class Gui : public IGui
{
public:
	~Gui();

	void Init() override;
	void Render() override;
	void Update() override;

	std::vector<Panel*>& GetPanels() { return m_panels; }
	DebugMenu* m_DebugMenu;

private:
	std::vector<Panel*> m_panels;
};
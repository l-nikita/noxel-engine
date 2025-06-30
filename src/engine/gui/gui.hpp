#pragma once

#include "igui.hpp"

class DebugMenu;

class Gui : public IGui
{
public:
	bool Init(SDL_Window* window, IEngine* engine) override;
	void Render() override;
	void Shutdown() override;

	std::vector<Panel*>& GetPanels() override { return m_panels; }
	DebugMenu* m_DebugMenu;

private:
	std::vector<Panel*> m_panels;

	SDL_Window* m_window = nullptr;
	IEngine* m_engine = nullptr;
};
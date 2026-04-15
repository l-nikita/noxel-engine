#pragma once

#include <vector>

class Panel;

class IGui
{
public:
	virtual ~IGui() = default;

	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void Update() = 0;

	virtual std::vector<Panel*>& GetPanels() = 0;
};

extern IGui* g_Gui;
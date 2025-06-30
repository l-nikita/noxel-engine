#pragma once

#include <vector>

class Panel;

class IGui
{
public:
	virtual ~IGui() = default;

	virtual bool Init() = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;

	virtual std::vector<Panel*>& GetPanels() = 0;
};
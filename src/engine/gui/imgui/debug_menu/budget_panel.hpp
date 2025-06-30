#pragma once

#include "gui/panel.hpp"
#include "common/profiler.hpp"

constexpr auto FPS_HISTORY_SIZE = 60;

class BudgetPanel : public Panel
{
public:
	void Draw();
	void DrawRow(Profiler* profiler);

private:
	float m_fps;
	int m_frameRateIndex;
	float m_frameRates[FPS_HISTORY_SIZE] = {};
};
#pragma once

#include "gui/panel.hpp"
#include "dbg/profiler.hpp"

constexpr auto FPS_HISTORY_SIZE = 60;

class BudgetPanel : public Panel
{
public:
	BudgetPanel();
	~BudgetPanel() override;

	void Draw();
	void DrawRow(Profiler::Profiler* prof);

private:
	float m_fps;
	int m_frameRateIndex;
	float m_frameRates[FPS_HISTORY_SIZE] = {};
};
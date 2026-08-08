#pragma once

#include "dbg/profiler.hpp"
#include "gui/panel.hpp"

constexpr auto FPS_HISTORY_SIZE = 60;

class BudgetPanel : public Panel
{
public:
    BudgetPanel();
    ~BudgetPanel() override;

    void Draw() override;
    static void DrawRow( const std::unique_ptr<profiler::Profiler>& prof );

private:
    float m_fps = 0.0f;
    int m_frameRateIndex = 0;
    float m_frameRates[ FPS_HISTORY_SIZE ] = {};
};

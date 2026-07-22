#pragma once

#include "gui/panel.hpp"

constexpr auto INPUT_SIZE = 128;

class ConsolePanel : public Panel
{
public:
    void Draw() override;
    void DrawMenuBar();
    void DrawLogWindow();
    void DrawInput();

    void ExecuteCommand( char* input );

private:
    bool m_timeStamp = false;
    bool m_autoScroll = true;

    bool m_scrollToBottom = true;

    char m_input[ INPUT_SIZE ] = {};
};

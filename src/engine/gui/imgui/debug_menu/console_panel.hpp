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

private:
    static std::vector<std::string> FindMatches( const std::string& input );

private:
    bool m_timeStamp = false;
    bool m_autoScroll = true;

    bool m_scrollToBottom = true;

    char m_buffer[ INPUT_SIZE ] = {};
    std::string m_input;
    int m_selectedSuggestion = -1;
};

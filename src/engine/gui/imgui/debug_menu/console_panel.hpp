#pragma once

#include <string>

#include "panel.hpp"

constexpr auto INPUT_SIZE = 128;

class ConsolePanel : public Panel
{
public:
	void Draw();
	void DrawMenuBar();
	void DrawLogWindow();
	void DrawInput();

	void ExecuteCommand(char* input);
private:
	bool m_timeStamp;
	bool m_autoScroll;

	bool m_scrollToBottom;

	char m_input[INPUT_SIZE];
};
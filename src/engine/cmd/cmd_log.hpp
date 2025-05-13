#pragma once

#include <string>
#include <ctime>

enum CMDLogType
{
	DEFAULT, // White
	INFO, // Blue
	WARNING, // Orange
	ERROR, // Red
};

struct CMDLog
{
	const std::string m_Text;
	const time_t m_Time;
	const CMDLogType m_Type;

	CMDLog(const std::string& text, time_t time, CMDLogType type)
		: m_Text(text), m_Time(time), m_Type(type) {}
};
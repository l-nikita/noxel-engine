#pragma once

#include <string>
#include <ctime>

#ifdef ERROR
#undef ERROR
#endif

enum CMDLogType
{
	DEFAULT, // White
	INFO, // Blue
	WARNING, // Orange
	ERROR, // Red
};

struct CMDLog
{
	const std::string Text;
	const time_t Time;
	const CMDLogType Type;

	CMDLog(const std::string& text, time_t time, CMDLogType type)
		: Text(text), Time(time), Type(type) {}
};
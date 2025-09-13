#pragma once

#include <string>
#include <ctime>

#ifdef ERROR
#undef ERROR
#endif

namespace CMD
{
	enum LogType
	{
		DEFAULT, // White
		INFO, // Blue
		WARNING, // Orange
		ERROR, // Red
		FATAL_ERROR,
	};

	enum LogCategory
	{
		NONE = 0,
		ENGINE,
		SERVER,
		CLIENT,
		NETWORKING,
		TOOLS,
		PHYSICS,
		RENDERING,
		ASSETSYSTEM,
		CONSOLE,
		INPUT,
	};

	static std::string LogTypeToString(LogType type)
	{
		switch (type)
		{
			case DEFAULT: return "";
			case INFO: return "INFO";
			case WARNING: return "WARNING";
			case ERROR: return "ERROR";
			case FATAL_ERROR: return "FATAL_ERROR";
		}

		return "UNKNOWN_TYPE";
	}

	static std::string LogCategoryToString(LogCategory cat)
	{
		switch (cat)
		{
			case NONE: return "";
			case ENGINE: return "ENGINE";
			case SERVER: return "SERVER";
			case CLIENT: return "CLIENT";
			case TOOLS: return "TOOLS";
			case PHYSICS: return "PHYSICS";
			case CONSOLE: return "CONSOLE";
			case RENDERING: return "RENDERING";
			case ASSETSYSTEM: return "ASSETSYSTEM";
			case NETWORKING: return "NETWORKING";
			case INPUT: return "INPUT";
		}

		return "UNKNOWN_CATEGORY";
	}

	struct Log
	{
		const std::string Text;
		const std::time_t Time;
		const LogType Type;
		const LogCategory Category;

		Log(const std::string& text, std::time_t time, LogType type, LogCategory cat)
			: Text(text), Time(time), Type(type), Category(cat) {}
	};
}
#include "cmd_log.hpp"

namespace CMD
{
    std::string LogTypeToString(LogType type)
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

	std::string LogCategoryToString(LogCategory cat)
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
}
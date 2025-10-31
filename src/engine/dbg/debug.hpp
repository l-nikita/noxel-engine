#pragma once

#include "cmd/cmd.hpp"
#include "profiler.hpp"

#define CMD_LOG(msg, type, category) CMD::CMD::GetInstance().LogMsg(std::string(msg), type, category);
#define CMD_LOG_WITH_FILE(msg, type, category) CMD::CMD::GetInstance().LogMsg(std::string(msg) + " [" + __FILE__ + ":" + std::to_string(__LINE__) + "]", type, category);

#define CMD_MSG(msg, category) CMD_LOG(msg, CMD::DEFAULT, category)
#define CMD_DMSG(msg) CMD_LOG(msg, CMD::DEFAULT, CMD::NONE)

#define CMD_INFO(msg, category) CMD_LOG(msg, CMD::INFO, category)
#define CMD_WARNING(msg, category) CMD_LOG(msg, CMD::WARNING, category)
#define CMD_ERROR(msg, category) CMD_LOG_WITH_FILE(msg, CMD::ERROR, category)

#define FATAL_ERROR(msg, category) \
{ \
	CMD_LOG_WITH_FILE(msg, CMD::FATAL_ERROR, category); \
	throw std::runtime_error(msg); \
}

#define PROF_START(name, category) Profiler::Start(name, category);
#define PROF_END(name) Profiler::End(name);
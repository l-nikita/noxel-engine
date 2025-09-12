#pragma once

#include "cmd/cmd.hpp"

#define CMD_LOG(msg, type, category) CMD::CMD::GetInstance().LogMsg(std::string(msg), type, category)
#define CMD_LOG_WITH_FILE(msg, type, category) CMD::CMD::GetInstance().LogMsg(std::string(msg) + " [" + __FILE__ + ":" + std::to_string(__LINE__) + "]", type, category)

#define CMD_MSG(msg, category) CMD_LOG(msg, CMD::LogType::DEFAULT, category)
#define CMD_DMSG(msg) CMD_LOG(msg, CMD::LogType::DEFAULT, CMD::NONE)

#define CMD_INFO(msg, category) CMD_LOG(msg, CMD::LogType::INFO, category)
#define CMD_WARNING(msg, category) CMD_LOG_WITH_FILE(msg, CMD::LogType::WARNING, category)
#define CMD_ERROR(msg, category) CMD_LOG_WITH_FILE(msg, CMD::LogType::ERROR, category)

#define FATAL_ERROR(msg, category) throw std::runtime_error(LogCategoryToString(category) + std::string("[FATAL ERROR]: ") + msg)
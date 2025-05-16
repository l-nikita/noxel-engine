#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>

#include "singleton.hpp"
#include "cmd_log.hpp"
#include "cmd_concommand.hpp"

class CMD : public Singleton<CMD>
{
private:
	std::vector<std::unique_ptr<CMDLog>> m_logs;
	std::unordered_map<std::string, CMDConCommand> m_commands;
public:
	void Setup();

	void RegisterCommand(const std::string& name, const std::string& desc, const std::function<void()>& func);
	bool IsCommandExist(const std::string& name);
	void Execute(const std::string& name);
	std::unordered_map<std::string, CMDConCommand> GetCommands() { return m_commands; }

	void Log(const std::string& msg, CMDLogType type);
	void ClearLogs();
	std::vector<std::unique_ptr<CMDLog>>& GetLogs() { return m_logs; }

	void LogHelp();
};

static void CMD_LOG(const std::string& msg, CMDLogType type) { CMD::GetInstance().Log(msg, type); }
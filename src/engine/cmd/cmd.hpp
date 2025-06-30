#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <optional>

#include "common/singleton.hpp"
#include "cmd_log.hpp"
#include "cmd_command.hpp"
#include "cmd_var.hpp"

using CMDCommands = std::unordered_map<std::string, CMDCommand>;
using CMDVars = std::unordered_map<std::string, CMDVar>;

class CMD : public Singleton<CMD>
{
private:
	std::vector<std::unique_ptr<CMDLog>> m_logs;
	CMDCommands m_commands;
	CMDVars m_vars;
public:
	void Setup();

	void RegisterCommand(const std::string& name, const std::string& desc, const std::function<void()>& func);
	bool IsCommandExist(const std::string& name);
	void Execute(const std::string& name);
	CMDCommands GetCommands() { return m_commands; }

	void RegisterVar(const std::string& name, const std::string& desc, const CMDVarValue& defValue, const CMDVarCallback& callback = nullptr);
	bool IsVarExist(const std::string& name);
	void SetVar(const std::string& name, const CMDVarValue& value);
	std::optional<CMDVarValue> GetVarValue(const std::string& name) const;

	std::vector<std::string> Tokenize(const std::string& input);
	CMDVars GetVars() { return m_vars; }

	CMDVarValue ConvertStringToVarValue(const std::string& input);

	void Log(const std::string& msg, CMDLogType type);
	void ClearLogs();
	std::vector<std::unique_ptr<CMDLog>>& GetLogs() { return m_logs; }

	void LogHelp();
};

static void CMD_LOG(const std::string& msg, CMDLogType type) { CMD::GetInstance().Log(msg, type); }
static void CMD_LOG_INFO(const std::string& msg) { CMD::GetInstance().Log(msg, CMDLogType::INFO); }
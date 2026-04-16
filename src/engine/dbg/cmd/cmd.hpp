#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <optional>
#include <memory>

#include "common/singleton.hpp"
#include "cmd_log.hpp"
#include "cmd_command.hpp"
#include "cmd_var.hpp"

namespace CMD
{
	using Commands = std::unordered_map<std::string, Command>;
	using Vars = std::unordered_map<std::string, Var>;

	class CMD : public Singleton<CMD>
	{
	public:
		void Setup();

		void RegisterCommand(const std::string& name, const std::string& desc, const std::function<void()>& func);
		bool IsCommandExist(const std::string& name);
		void Execute(const std::string& name);
		Commands GetCommands() { return m_commands; }

		void RegisterVar(const std::string& name, const std::string& desc, const VarValue& defValue, const VarCallback& callback = nullptr);
		bool IsVarExist(const std::string& name);
		void SetVar(const std::string& name, const VarValue& value);
		std::optional<VarValue> GetVarValue(const std::string& name) const;

		std::vector<std::string> Tokenize(const std::string& input);
		Vars GetVars() { return m_vars; }

		VarValue ConvertStringToVarValue(const std::string& input);

		void LogMsg(const std::string& msg, LogType type, LogCategory category);
		void ClearLogs();
		std::vector<std::unique_ptr<Log>>& GetLogs() { return m_logs; }

		void LogHelp();

	private:
		std::vector<std::unique_ptr<Log>> m_logs;
		Commands m_commands;
		Vars m_vars;
	};

	void RegisterCommand(const std::string& name, const std::string& desc, const std::function<void()>& func);
	void RegisterVar(const std::string& name, const std::string& desc, const VarValue& defValue, const VarCallback& callback = nullptr);
	void Execute(const std::string& name);
}
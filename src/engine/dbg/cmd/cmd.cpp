#include <algorithm>
#include <charconv>
#include <optional>
#include <format>
#include <iomanip> 

#include "cmd.hpp"

namespace CMD
{
	void CMD::Setup()
	{
		RegisterCommand("help", "", [this]() {
			LogHelp();
		});

		RegisterCommand("clear", "Clear console logs.", [this]() {
			ClearLogs();
		});

		RegisterVar("test_var", "test", false);
		RegisterVar("test_var_str", "test", "Sex");
		RegisterVar("test_var_int", "test", 100);
		RegisterVar("test_var_fl", "test", 3.0f);
	}

	void CMD::RegisterCommand(const std::string& name, const std::string& desc, const std::function<void()>& func)
	{
		if (IsCommandExist(name))
		{
			LogMsg("Couldn't register command [" + name + "] because it already exists.", WARNING, CONSOLE);
			return;
		}

		m_commands.emplace(name, Command(name, desc, func));
	}

	bool CMD::IsCommandExist(const std::string& name)
	{
		auto iterator = m_commands.find(name);
		if (iterator != m_commands.end())
			return true;

		return false;
	}

	void CMD::RegisterVar(const std::string& name, const std::string& desc, const VarValue& defValue, const VarCallback& callback)
	{
		if (IsVarExist(name))
		{
			LogMsg("Couldn't register console var [" + name + "] because it already exists.", WARNING, CONSOLE);
			return;
		}

		m_vars.emplace(name, Var(name, desc, defValue, callback));
	}

	bool CMD::IsVarExist(const std::string& name)
	{
		auto iterator = m_vars.find(name);
		if (iterator != m_vars.end())
			return true;

		return false;
	}

	void CMD::SetVar(const std::string& name, const VarValue& value)
	{
		if (!IsVarExist(name)) return;

		auto& var = m_vars[name];

		if (var.Value.index() != value.index())
		{
			LogMsg("Type mismatch for variable [" + name + "]", WARNING, CONSOLE);
			return;
		}

		if (var.Callback)
			var.Callback(var.Value, value);

		var.Value = value;
	}

	std::optional<VarValue> CMD::GetVarValue(const std::string& name) const
	{
		auto it = m_vars.find(name);
		if (it != m_vars.end())
			return it->second.Value;

		return std::nullopt;
	}

	void CMD::Execute(const std::string& name)
	{
		auto tokens = Tokenize(name);
		if (tokens.empty())
			return;

		const std::string& cmd = tokens[0];

		bool isCommand = IsCommandExist(cmd);
		bool isConVar = IsVarExist(cmd);

		if (!isCommand && !isConVar)
		{
			LogMsg("Unknown command: " + cmd, DEFAULT, CONSOLE);
			return;
		}

		LogMsg("] " + name, DEFAULT, CONSOLE);

		if (isCommand)
		{
			m_commands[cmd].Func();
		}
		else if (isConVar)
		{
			if (tokens.size() == 1)
			{
				auto& var = m_vars[cmd];
				LogMsg(cmd + " = " + var.ValueToString(), DEFAULT, CONSOLE);
			}
			else
			{
				std::string valueStr = tokens[1];
				auto value = ConvertStringToVarValue(valueStr);
				SetVar(cmd, value);
			}
		}
	}

	VarValue CMD::ConvertStringToVarValue(const std::string& input)
	{
		std::string trimmed = input;
		trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
		trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

		std::string lower = trimmed;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

		// Bool check
		if (lower == "true")
			return true;

		if (lower == "false")
			return false;

		// Try int
		int intVal;
		auto [p1, ec1] = std::from_chars(trimmed.data(), trimmed.data() + trimmed.size(), intVal);
		if (ec1 == std::errc() && p1 == trimmed.data() + trimmed.size())
			return intVal;

		// Try float
		try
		{
			size_t idx;
			float floatVal = std::stof(trimmed, &idx);
			if (idx == trimmed.size())
				return floatVal;
		}
		catch (...)
		{
		}

		return trimmed;
	}

	std::vector<std::string> CMD::Tokenize(const std::string& input)
	{
		std::vector<std::string> tokens;
		bool inQuotes = false;
		std::string current;

		for (int i = 0; i < input.size(); ++i)
		{
			char c = input[i];

			if (c == '"')
			{
				inQuotes = !inQuotes;
				continue;  // skip the quote itself
			}

			if (c == ' ' && !inQuotes)
			{
				if (!current.empty())
				{
					tokens.push_back(current);
					current.clear();
				}
			}
			else
				current += c;
		}

		if (!current.empty())
			tokens.push_back(current);

		return tokens;
	}

	void CMD::LogMsg(const std::string& msg, LogType type, LogCategory category)
	{
		auto t = time(nullptr);
		m_logs.emplace_back(std::make_unique<Log>(msg, t, type, category));

		std::tm* localTime = std::localtime(&t);
		std::ostringstream oss;
		oss << std::put_time(localTime, "%H:%M:%S");
		std::string formattedT = oss.str();

		std::string typeStr = LogTypeToString(type);
		std::string categoryStr = (category != NONE) ? LogCategoryToString(category) : "";

		std::cout << std::format("[ {:8} | {:12} | {:6} ] {}\n", typeStr, categoryStr, formattedT, msg);
	}

	void CMD::ClearLogs()
	{
		m_logs.clear();
	}

	void CMD::LogHelp()
	{
		LogMsg("//-------- COMMANDS --------", DEFAULT, CONSOLE);
		for (auto it = m_commands.begin(); it != m_commands.end(); ++it)
		{
			if (it->first != "help")
			{
				auto& desc = it->second.Desc;
				if(desc.empty())
					LogMsg(it->first, DEFAULT, CONSOLE);
				else
					LogMsg(it->first + " - " + it->second.Desc, DEFAULT, CONSOLE);
			}
		}

		LogMsg("//-------- VARS --------", DEFAULT, CONSOLE);
		for (auto it = m_vars.begin(); it != m_vars.end(); ++it)
		{
			auto& desc = it->second.Desc;
			if (desc.empty())
				LogMsg(it->first + " = " + it->second.ValueToString(), DEFAULT, CONSOLE);
			else
				LogMsg(it->first + " = " + it->second.ValueToString() + " [ default: " + it->second.DefValueToString() + " ] - " + it->second.Desc, DEFAULT, CONSOLE);
		}
	}
}
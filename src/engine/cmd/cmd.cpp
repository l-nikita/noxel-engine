#include "cmd.hpp"

void CMD::Setup()
{
	RegisterCommand("help", "", [this]() {
		LogHelp();
	});	
	
	RegisterCommand("clear", "Clear console logs.", [this]() {
		ClearLogs();
	});
}

void CMD::RegisterCommand(const std::string& name, const std::string& desc, const std::function<void()>& func)
{
	if (IsCommandExist(name))
	{
		Log("Couldn't register command ["+name+"] because it already exists.", CMDLogType::WARNING);
		return;
	}

	m_commands.emplace(name, CMDConCommand(name, desc, func));
}

bool CMD::IsCommandExist(const std::string& name)
{
	auto iterator = m_commands.find(name);
	if (iterator != m_commands.end())
		return true;

	return false;
}

void CMD::Execute(const std::string& name)
{
	if (!IsCommandExist(name))
	{
		Log("Unknown command: " + name, CMDLogType::DEFAULT);
		return;
	}

	auto iterator = m_commands.find(name);
	if (iterator != m_commands.end())
	{
		Log("] " + name, CMDLogType::DEFAULT);
		iterator->second.m_Func();
	}
}

void CMD::Log(const std::string& msg, CMDLogType type)
{
	m_logs.emplace_back(std::make_unique<CMDLog>(msg, time(nullptr), type));
	if(type == CMDLogType::ERROR)
		std::cerr << msg << std::endl;
	else
		std::cout << msg << std::endl;
}

void CMD::ClearLogs()
{
	m_logs.clear();
}

void CMD::LogHelp()
{
	for (auto it = m_commands.begin(); it != m_commands.end(); ++it)
	{
		if (it->first != "help")
			Log(it->first + " - " + it->second.m_Desc, CMDLogType::DEFAULT);
	}
}
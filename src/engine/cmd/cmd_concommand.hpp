#pragma once

#include <string>
#include <functional>

struct CMDConCommand
{
	const std::string m_Name;
	const std::string m_Desc;
	const std::function<void()> m_Func;

	CMDConCommand(const std::string& name, const std::string& desc, const std::function<void()>& func)
		: m_Name(name), m_Desc(desc), m_Func(func) {}
};
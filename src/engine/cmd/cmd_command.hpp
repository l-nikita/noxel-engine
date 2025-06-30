#pragma once

#include <string>
#include <functional>

struct CMDCommand
{
	const std::string Name;
	const std::string Desc;
	const std::function<void()> Func;

	CMDCommand() = default;
	CMDCommand(const std::string& name, const std::string& desc, const std::function<void()>& func)
		: Name(name), Desc(desc), Func(func) {}
};
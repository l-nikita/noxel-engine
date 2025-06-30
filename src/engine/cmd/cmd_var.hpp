#pragma once

#include <string>
#include <variant>
#include <functional>
#include <sstream>

using CMDVarValue = std::variant<int, float, bool, std::string>;
using CMDVarCallback = std::function<void(CMDVarValue, CMDVarValue)>;

struct CMDVar
{
	const std::string Name;
	const std::string Desc;
	const CMDVarCallback Callback;
	const CMDVarValue DefaultValue;
	CMDVarValue Value;

	CMDVar() = default;

	CMDVar(const std::string& name,
		const std::string& desc,
		const CMDVarValue& value,
		const CMDVarCallback& callback = nullptr)
		: Name(name), Desc(desc), Callback(callback), DefaultValue(value), Value(value) {
	}

	std::string ValueToString() const
	{
		return std::visit([](auto&& val) -> std::string {
			std::ostringstream oss;
			oss << std::boolalpha << val;
			return oss.str();
			}, Value);
	}
};

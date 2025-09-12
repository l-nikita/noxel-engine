#pragma once

#include <string>
#include <variant>
#include <functional>
#include <sstream>

namespace CMD
{
	using VarValue = std::variant<int, float, bool, std::string>;
	using VarCallback = std::function<void(VarValue, VarValue)>;

	struct Var
	{
		const std::string Name;
		const std::string Desc;
		const VarCallback Callback;
		const VarValue DefaultValue;
		VarValue Value;

		Var() = default;

		Var(const std::string& name,
			const std::string& desc,
			const VarValue& value,
			const VarCallback& callback = nullptr)
			: Name(name), Desc(desc), Callback(callback), DefaultValue(value), Value(value) 
		{}

		std::string ValueToString() const
		{
			return std::visit([](auto&& val) -> std::string {
				std::ostringstream oss;
				oss << std::boolalpha << val;
				return oss.str();
				}, Value);
		}
	};
}
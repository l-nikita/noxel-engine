#pragma once

#include <string>
#include <functional>

namespace CMD
{
    struct Command
    {
        const std::string Name;
        const std::string Desc;
        const std::function<void()> Func;

        Command() = default;

        Command( const std::string& name, const std::string& desc, const std::function<void()>& func )
            : Name(name), Desc(desc), Func(func)
        {
        }
    };
}
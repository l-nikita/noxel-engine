#pragma once

#include <expected>
#include <string>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using SysInit = std::expected<void, std::string>;

class ISystem
{
public:
    virtual ~ISystem() = default;

    virtual SysInit Initialize() = 0;
    virtual void Shutdown() = 0;
};

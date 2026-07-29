#pragma once

#include <string>

#include "filesystem/ifilesystem.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct ShaderData
{
    std::string Name;
    FPath PixelShaderPath;
    FPath VertexShaderPath;
};

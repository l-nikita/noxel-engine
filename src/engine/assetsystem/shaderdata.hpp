#pragma once

#include <string>

#include "filesystem/ifilesystem.hpp"

namespace assets
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct ShaderData : IAssetData
    {
        std::string Name;
        fs::Path PixelShaderPath;
        fs::Path VertexShaderPath;
    };
}

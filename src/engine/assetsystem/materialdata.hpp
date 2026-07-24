#pragma once

#include <string>
#include <nlohmann/json.hpp>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct MaterialData
{
    std::string Shader;
    bool IsCubemap = false;
    nlohmann::json Parameters;
};

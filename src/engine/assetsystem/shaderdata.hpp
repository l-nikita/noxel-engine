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
        std::string SourceLanguage;

        fs::Path PixelShaderPath;
        fs::Path VertexShaderPath;

        struct TextureParam
        {
            std::string VarBind;
            std::string VarType;
        };

        std::unordered_map<std::string, TextureParam> Textures;

        struct SamplerParam
        {
            std::string MinFilter;
            std::string MagFilter;
            std::string MipFilter;

            std::string AddressU;
            std::string AddressV;
            std::string AddressW;
        };

        std::unordered_map<std::string, SamplerParam> Samplers;
    };
}

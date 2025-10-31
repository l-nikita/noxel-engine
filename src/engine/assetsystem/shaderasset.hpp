#pragma once

#include <string>
#include "renderer/ishaderprogram.hpp"
#include "base_asset.hpp"
#include "asset_system.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static std::string LoadShaderSource(const std::string& shaderType, const std::string& assetPath)
{
    std::ifstream file(assetPath);
    std::ostringstream shaderStream;
    bool isInShader = false;

    if (!file)
    {
        CMD_ERROR("Failed to load shader source file: " + assetPath, CMD::ASSETSYSTEM);
        return "";
    }

    std::string line;
    while (std::getline(file, line))
    {
        shaderStream << line << '\n';
    }

    /*
    std::string line;
    while (std::getline(file, line))
    {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
        trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

        if (!isInShader && trimmed == shaderType)
        {
            isInShader = true;
            continue;
        }

        if (isInShader)
        {
            if (trimmed == "[")
                continue;

            if (trimmed == "]")
                break;

            shaderStream << line << '\n';
        }
    }*/

    return shaderStream.str();
}

class ShaderAsset : public BaseAsset
{
private:
    std::string m_shaderPixelSource;
    std::string m_shaderVertexSource;

public:
    AssetType GetType() const override { return Type; }
    static constexpr AssetType Type = AssetType::SHADER;

    ShaderAsset(const std::string& name, const std::string& assetPath)
    {
        m_name = name;
        m_assetPath = assetPath;

        m_shaderPixelSource = LoadShaderSource("PixelShader", assetPath + "_ps.hlsl");
        m_shaderVertexSource = LoadShaderSource("VertexShader", assetPath + "_vs.hlsl");
    }

    std::string GetShaderSource(ShaderType type) const
    {
        if (type == VERTEX)
            return m_shaderVertexSource;
        else if (type == PIXEL)
            return m_shaderPixelSource;
        else
            CMD_ERROR("Unsupported shader type.", CMD::ASSETSYSTEM);

        return "";
    }

#pragma region CreateNew/Get
    static ShaderAsset* CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto asset = new ShaderAsset(name, assetPath);
        g_Assets->RegisterAsset(asset);
        return asset;
    }

    static ShaderAsset* Get(const std::string& name)
    {
        auto& assets = g_Assets->GetAssets(Type);
        auto it = assets.find(name);

        if (it == assets.end())
        {
            CMD_ERROR("Couldn't find shader [" + name + "].", CMD::ASSETSYSTEM);
            return GetError();
        }

        return static_cast<ShaderAsset*>(it->second);
    }
#pragma endregion


    static ShaderAsset* m_error;

    //TODO: Make error shader
    static ShaderAsset* GetError()
    {
        auto asset = ShaderAsset::Get("colorpass");
        assert(asset);
        return asset;
    }
};

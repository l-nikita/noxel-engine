#pragma once

#include <fstream>
#include <string>
#include "assetsystem.hpp"
#include "baseasset.hpp"
#include "dbg/debug.hpp"
#include "renderer/ishaderprogram.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static std::string LoadShaderSource( const std::string& assetPath )
{
    std::ifstream file(assetPath);
    std::ostringstream shaderStream;

    if ( !file )
    {
        CMD_ERROR("Failed to load shader source file: " + assetPath, CMD::ASSETSYSTEM);
        return "";
    }

    std::string line;
    while ( std::getline(file, line) )
    {
        shaderStream << line << '\n';
    }

    return shaderStream.str();
}

class ShaderAsset : public BaseAsset
{
    std::string m_shaderPixelSource;
    std::string m_shaderVertexSource;

public:
    AssetType GetType() const override
    {
        return Type;
    }

    static constexpr auto Type = AssetType::SHADER;

    ShaderAsset( const std::string& name, const std::string& assetPath )
    {
        m_name = name;
        m_assetPath = assetPath;

        m_shaderPixelSource = LoadShaderSource(assetPath + "_ps.hlsl");
        m_shaderVertexSource = LoadShaderSource(assetPath + "_vs.hlsl");
    }

    std::string GetShaderSource( ShaderType type ) const
    {
        if ( type == VERTEX )
            return m_shaderVertexSource;

        if ( type == PIXEL )
            return m_shaderPixelSource;

        CMD_ERROR("Unsupported shader type.", CMD::ASSETSYSTEM);

        return "";
    }

#pragma region CreateNew/Get
    static ShaderAsset* CreateNew( const std::string& name, const std::string& assetPath )
    {
        const auto asset = new ShaderAsset(name, assetPath);
        g_Assets->RegisterAsset(asset);
        return asset;
    }

    static ShaderAsset* Get( const std::string& name )
    {
        auto& assets = g_Assets->GetAssets(Type);
        const auto it = assets.find(name);

        if ( it == assets.end() )
        {
            CMD_ERROR("Couldn't find shader [" + name + "].", CMD::ASSETSYSTEM);
            return GetError();
        }

        return dynamic_cast<ShaderAsset*>(it->second);
    }
#pragma endregion


    static ShaderAsset* m_error;

    //TODO: Make error shader
    static ShaderAsset* GetError()
    {
        const auto asset = ShaderAsset::Get("colorpass");
        assert(asset);
        return asset;
    }
};

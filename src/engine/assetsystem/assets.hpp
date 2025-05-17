#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <lodepng.h>

#include "base_asset.hpp"
#include "assets_system.hpp"
#include "cmd.hpp"

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
        CMD_LOG("Failed to load shader source file: " + assetPath, CMDLogType::ERROR);
        return "";
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find(shaderType) != std::string::npos)
        {
            isInShader = true;
        }

        if (isInShader)
        {
            if (line.find("]") != std::string::npos)
                break;

            if (line.find("[") == std::string::npos && line.find(shaderType) == std::string::npos)
                shaderStream << line << '\n';
        }
    }

    return shaderStream.str();
}

class ShaderAsset : public BaseAsset
{
private:
    std::string m_shaderFragSource;
    std::string m_shaderVertSource;

public:
    AssetType GetType() const override { return Type; }
    static constexpr AssetType Type = AssetType::SHADER;

    ShaderAsset(const std::string& name, const std::string& assetPath)
    {
        m_name = name;
        m_assetPath = assetPath;
        m_shaderFragSource = LoadShaderSource("FragmentShader", assetPath);
        m_shaderVertSource = LoadShaderSource("VertexShader", assetPath);
    }

    std::string GetShaderSource(GLenum type) const
    {
        if (type == GL_VERTEX_SHADER)
            return m_shaderVertSource;
        else if (type == GL_FRAGMENT_SHADER)
            return m_shaderFragSource;
        else
            CMD_LOG("Unsupported shader type.", CMDLogType::ERROR);

        return "";
    }

    static std::shared_ptr<ShaderAsset> CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto shader = std::make_shared<ShaderAsset>(name, assetPath);
        AssetsSystem::GetInstance().RegisterAsset(shader);
        return shader;
    }

    static std::shared_ptr<ShaderAsset> Get(const std::string& name)
    {
        auto shaders = AssetsSystem::GetInstance().GetAssets<ShaderAsset>();
        auto it = shaders.find(name);

        if (it == shaders.end())
            return nullptr;

        auto casted = std::dynamic_pointer_cast<ShaderAsset>(it->second);
        return casted;
    }
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct TextureData
{
    std::vector<unsigned char> Image;
    int Width;
    int Height;
    GLenum Format;
};

class TextureAsset : public BaseAsset
{
public:
    AssetType GetType() const override { return Type; }
    static constexpr AssetType Type = AssetType::TEXTURE;

    TextureAsset(const std::string& name, const std::string& assetPath)
    {
        m_name = name;
        m_assetPath = assetPath;
    }

    TextureData Load()
    {
        TextureData data;

        std::vector<unsigned char> image;
        unsigned width, height;
        
        // PNG -> RGBA
        unsigned error = lodepng::decode(data.Image, width, height, m_assetPath);
        
        if (error)
        {
            CMD_LOG("PNG decode error " + std::to_string(error) + ": " + lodepng_error_text(error), CMDLogType::ERROR);
            return {};
        }

        data.Format = GL_RGBA;
        data.Width = width;
        data.Height = height;

        return data;
    }

    static std::shared_ptr<TextureAsset> CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto shader = std::make_shared<TextureAsset>(name, assetPath);
        AssetsSystem::GetInstance().RegisterAsset(shader);
        return shader;
    }

    static std::shared_ptr<TextureAsset> Get(const std::string& name)
    {
        auto shaders = AssetsSystem::GetInstance().GetAssets<TextureAsset>();
        auto it = shaders.find(name);

        if (it == shaders.end())
            return nullptr;

        auto casted = std::dynamic_pointer_cast<TextureAsset>(it->second);
        return casted;
    }
};
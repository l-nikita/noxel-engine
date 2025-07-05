#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stb_image.h>

#include "renderer/ishader.hpp"
#include "base_asset.hpp"
#include "assets_system.hpp"
#include "cmd/cmd.hpp"

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
        CMD_LOG("Failed to load shader source file: " + assetPath, ERROR);
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

    std::string GetShaderSource(ShaderType type) const
    {
        if (type == VERTEX)
            return m_shaderVertSource;
        else if (type == PIXEL)
            return m_shaderFragSource;
        else
            CMD_LOG("Unsupported shader type.", ERROR);

        return "";
    }

#pragma region CreateNew/Get
    static std::shared_ptr<ShaderAsset> CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto asset = std::make_shared<ShaderAsset>(name, assetPath);
        g_Assets->RegisterAsset(asset);
        return asset;
    }

    static std::shared_ptr<ShaderAsset> Get(const std::string& name)
    {
        auto assets = g_Assets->GetAssets(Type);
        auto it = assets.find(name);

        if (it == assets.end())
            return nullptr;

        return std::dynamic_pointer_cast<ShaderAsset>(it->second);
    }
#pragma endregion
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct TextureData
{
    unsigned char* Image = nullptr;
    int Width;
    int Height;
    int Channels = 0;

    TextureData() = default;
    ~TextureData() { if (Image) stbi_image_free(Image); }

    // disable copying
    TextureData(const TextureData&) = delete;
    TextureData& operator=(const TextureData&) = delete;

    // move constructor to transfer ownership
    TextureData(TextureData&& other) noexcept
        : Image(other.Image), Width(other.Width), Height(other.Height), Channels(other.Channels)
    {
        other.Image = nullptr;
        other.Width = 0;
        other.Height = 0;
        other.Channels = 0;
    }

    TextureData& operator=(TextureData&& other) noexcept
    {
        if (this != &other)
        {
            if (Image) stbi_image_free(Image);

            Image = other.Image;
            Width = other.Width;
            Height = other.Height;
            Channels = other.Channels;

            other.Image = nullptr;
            other.Width = 0;
            other.Height = 0;
            other.Channels = 0;
        }
        return *this;
    }
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
        if (m_assetPath.empty())
        {
            CMD_LOG("Asset path is null or empty!!!", ERROR);
            return {};
        }

        int width, height, channels;
        unsigned char* image = stbi_load(m_assetPath.string().c_str(), &width, &height, &channels, 0);
        if (!image)
        {
            CMD_LOG("Couln't load texture ["+ m_assetPath.string()+"]!", ERROR);
            return {};
        }

        TextureData data{};
        data.Image = image;
        data.Width = width;
        data.Height = height;
        data.Channels = channels;

        return data;
    }

#pragma region CreateNew/Get
    static std::shared_ptr<TextureAsset> CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto asset = std::make_shared<TextureAsset>(name, assetPath);
        g_Assets->RegisterAsset(asset);
        return asset;
    }

    static std::shared_ptr<TextureAsset> CreateOrGet(const std::string& name, const std::string& assetPath)
    {
        auto asset = g_Assets->HasAsset(Type, name);
        if (asset)
            return std::dynamic_pointer_cast<TextureAsset>(asset);

        auto texAsset = std::make_shared<TextureAsset>(name, assetPath);
        g_Assets->RegisterAsset(texAsset);
        return texAsset;
    }

    static std::shared_ptr<TextureAsset> Get(const std::string& name)
    {
        auto assets = g_Assets->GetAssets(Type);
        auto it = assets.find(name);

        if (it == assets.end())
        {
            CMD_LOG("Couldn't find texture [" + name + "].", WARNING);
            return nullptr;
        }

        auto casted = std::dynamic_pointer_cast<TextureAsset>(it->second);
        return casted;
    }

    static std::shared_ptr<TextureAsset> GetError() { return TextureAsset::Get("error"); }
#pragma endregion
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using json = nlohmann::json;

struct MaterialData
{
    std::string Shader;
    std::string BaseTexture;
};

//inline void to_json(json& j, const MaterialData& data) 
//{
//    j = json{
//        {"shader", data.Shader},
//        {"basetexture", data.BaseTexture}
//    };
//}

inline void from_json(const json& j, MaterialData& data) 
{
    data.Shader = j.value("shader", "default");
    data.BaseTexture = j.value("basetexture", "");
}

class MaterialAsset : public BaseAsset
{
private:
    std::shared_ptr<TextureAsset> m_textureAsset;
    std::shared_ptr<ShaderAsset> m_shaderAsset;

public:
    AssetType GetType() const override { return Type; }
    static constexpr AssetType Type = AssetType::MATERIAL;

    MaterialAsset(const std::string& name, const std::string& assetPath)
    {
        m_name = name;
        m_assetPath = assetPath;

        std::ifstream inFile(m_assetPath);

        if (!inFile.is_open()) 
        {
            CMD_LOG("Couldn't open file [" + m_assetPath.string() + "].", ERROR);
            return;
        }

        json j;
        inFile >> j;
        inFile.close();

        MaterialData data{};

        try 
        {
            data = j.get<MaterialData>();
        }
        catch (const json::exception& e) 
        {
            CMD_LOG(std::string("JSON parse error: ") + e.what(), ERROR);
            return;
        }

        if (!data.BaseTexture.empty())
        {
            std::filesystem::path baseTexturePath(data.BaseTexture);

            auto fullTexturePath = g_Assets->GetMaterialsPath() / baseTexturePath;

            if (std::filesystem::exists(fullTexturePath))
                m_textureAsset = TextureAsset::CreateOrGet(fullTexturePath.stem().string(), fullTexturePath.string());
            else
            {
                CMD_LOG("Texture not found: " + fullTexturePath.string(), WARNING);
                m_textureAsset = TextureAsset::GetError();
            }
        }

        m_shaderAsset = ShaderAsset::Get(data.Shader);
    }

    std::shared_ptr<TextureAsset> TextureAsset() const { return m_textureAsset; }
    std::shared_ptr<ShaderAsset> ShaderAsset() const { return m_shaderAsset; }

#pragma region CreateNew/Get
    static std::shared_ptr<MaterialAsset> CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto asset = std::make_shared<MaterialAsset>(name, assetPath);
        g_Assets->RegisterAsset(asset);
        return asset;
    }

    static std::shared_ptr<MaterialAsset> Get(const std::string& name)
    {
        auto assets = g_Assets->GetAssets(Type);
        auto it = assets.find(name);

        if (it == assets.end())
            return nullptr;

        auto casted = std::dynamic_pointer_cast<MaterialAsset>(it->second);
        return casted;
    }

    static std::shared_ptr<MaterialAsset> GetError() { return MaterialAsset::Get("error"); }
#pragma endregion
};
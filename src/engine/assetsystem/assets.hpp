#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stb_image.h>

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
        auto asset = ShaderAsset::Get("default");
        assert(asset != nullptr);
        return asset;
    }
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct TextureData
{
    unsigned char* Image = nullptr;
    int Width = 0;
    int Height = 0;
    int Channels = 0;

    TextureData() = default;
    ~TextureData() { Release(); }

    // disable copying
    TextureData(const TextureData&) = delete;
    TextureData& operator=(const TextureData&) = delete;

    void Release() 
    {  
        if (Image)
        {
            stbi_image_free(Image);
            Image = nullptr;
            Width = 0;
            Height = 0;
            Channels = 0;
        }
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

    TextureData* Load()
    {
        if (m_assetPath.empty())
        {
            CMD_ERROR("Asset path is null or empty!!!", CMD::ASSETSYSTEM);
            return nullptr;
        }

        int width, height, channels;
        unsigned char* image = stbi_load(m_assetPath.string().c_str(), &width, &height, &channels, 4);
        if (!image)
        {
            CMD_ERROR("Couldn't load texture ["+ m_assetPath.string()+"]!", CMD::ASSETSYSTEM);
            return nullptr;
        }

        TextureData* data = new TextureData();
        data->Image = image;
        data->Width = width;
        data->Height = height;
        data->Channels = channels;

        return data;
    }

#pragma region CreateNew/Get
    static TextureAsset* CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto asset = new TextureAsset(name, assetPath);
        g_Assets->RegisterAsset(asset);
        return asset;
    }

    static TextureAsset* CreateOrGet(const std::string& name, const std::string& assetPath)
    {
        auto asset = g_Assets->GetAsset(Type, name);
        if (asset)
            return static_cast<TextureAsset*>(asset);

        auto texAsset = new TextureAsset(name, assetPath);
        g_Assets->RegisterAsset(texAsset);
        return texAsset;
    }

    static TextureAsset* Get(const std::string& name)
    {
        auto& assets = g_Assets->GetAssets(Type);
        auto it = assets.find(name);

        if (it == assets.end())
        {
            CMD_ERROR("Couldn't find texture [" + name + "].", CMD::ASSETSYSTEM);
            return GetError();
        }

        return static_cast<TextureAsset*>(it->second);
    }
#pragma endregion

    static TextureAsset* m_error;

    static TextureAsset* GetError()
    {
        assert(m_error != nullptr);
        return m_error;
    }
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
    TextureAsset* m_textureAsset = nullptr;
    ShaderAsset* m_shaderAsset = nullptr;

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
            CMD_ERROR("Couldn't open file [" + m_assetPath.string() + "].", CMD::ASSETSYSTEM);
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
            CMD_ERROR(std::string("JSON parse error: ") + e.what(), CMD::ASSETSYSTEM);
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
                CMD_ERROR("Texture not found: " + fullTexturePath.string(), CMD::ASSETSYSTEM);
                m_textureAsset = TextureAsset::GetError();
            }
        }

        auto shaderAsset = ShaderAsset::Get(data.Shader); // TODO: Make fallback shader in code
        if (shaderAsset == nullptr)
            FATAL_ERROR("Shader ["+ data.Shader + "] not found!", CMD::ASSETSYSTEM);

        m_shaderAsset = shaderAsset;
    }

    TextureAsset* TextureAsset() const { return m_textureAsset; }

    ShaderAsset* ShaderAsset() const 
    { 
        assert(m_shaderAsset != nullptr); // Shader required always
        return m_shaderAsset; 
    }

#pragma region CreateNew/Get
    static MaterialAsset* CreateNew(const std::string& name, const std::string& assetPath)
    {
        auto asset = new MaterialAsset(name, assetPath);
        g_Assets->RegisterAsset(asset);
        return asset;
    }

    static MaterialAsset* Get(const std::string& name)
    {
        auto& assets = g_Assets->GetAssets(Type);
        auto it = assets.find(name);

        if (it == assets.end())
        {
            CMD_ERROR("Couldn't find material [" + name + "].", CMD::ASSETSYSTEM);
            return GetError();
        }

        auto casted = static_cast<MaterialAsset*>(it->second);
        return casted;
    }
#pragma endregion

    static MaterialAsset* m_error;

    static MaterialAsset* GetError()
    {
        assert(m_error != nullptr);
        return m_error;
    }
};
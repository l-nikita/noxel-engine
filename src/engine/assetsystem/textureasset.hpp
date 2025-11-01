#pragma once

#include <stb_image.h>
#include "base_asset.hpp"
#include "asset_system.hpp"
#include "dbg/debug.hpp"

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

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
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
            CMD_ERROR("Couldn't load texture [" + m_assetPath.string() + "]!", CMD::ASSETSYSTEM);
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
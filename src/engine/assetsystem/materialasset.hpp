#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include "assetsystem.hpp"
#include "baseasset.hpp"
#include "shaderasset.hpp"
#include "textureasset.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct MaterialData
{
    std::string Shader;
    bool IsCubemap = false;
    nlohmann::json Parameters;
};

inline void from_json( const nlohmann::json& json, MaterialData& data )
{
    data.Shader = json.value( "shader", "geometrypass" );
    data.IsCubemap = json.value( "iscubemap", false );

    if ( json.contains( "parameters" ) )
        data.Parameters = json[ "parameters" ];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using TextureList = std::unordered_map<std::string, TextureAsset*>;

class MaterialAsset : public BaseAsset
{
private:
    ShaderAsset* m_shaderAsset = nullptr;
    MaterialData m_data;

    TextureList m_textureAssets;

public:
    AssetType GetType() const override
    {
        return Type;
    }

    static constexpr auto Type = AssetType::MATERIAL;

    MaterialAsset( const std::string& name, const std::string& assetPath )
    {
        m_name = name;
        m_assetPath = assetPath;

        std::ifstream inFile( m_assetPath );

        if ( !inFile.is_open() )
        {
            CMD_ERROR( "Couldn't open file [" + m_assetPath.string() + "].", CMD::ASSETSYSTEM );
            return;
        }

        nlohmann::json json;
        inFile >> json;

        try
        {
            m_data = json.get<MaterialData>();
        } catch ( const nlohmann::json::exception& e )
        {
            CMD_ERROR( "JSON parse error: " + std::string(e.what()), CMD::ASSETSYSTEM );
            return;
        }

        inFile.close();

        for ( const auto& [ key, value ]: m_data.Parameters.items() )
        {
            if ( value.is_string() )
            {
                std::string texPath = value;
                auto fullPath = g_Assets->GetMaterialsPath() / texPath;

                TextureAsset* texAsset = nullptr;

                if ( std::filesystem::exists( fullPath ) )
                {
                    texAsset = TextureAsset::CreateOrGet( fullPath.stem().string(), fullPath.string() );
                } else
                {
                    CMD_ERROR( "Texture not found: " + fullPath.string(), CMD::ASSETSYSTEM );
                    texAsset = TextureAsset::GetError();
                }

                m_textureAssets[ key ] = texAsset;
            }
        }

        auto shaderAsset = ShaderAsset::Get( m_data.Shader ); // TODO: Make fallback shader in code
        if ( shaderAsset == nullptr )
            FATAL_ERROR( "Shader [" + m_data.Shader + "] not found!", CMD::ASSETSYSTEM );

        m_shaderAsset = shaderAsset;
    }

    ShaderAsset* GetShaderAsset() const
    {
        assert( m_shaderAsset ); // Shader required always
        return m_shaderAsset;
    }

    MaterialData& GetData()
    {
        return m_data;
    }

    bool IsCubemap() const
    {
        return m_data.IsCubemap;
    }

    std::string GetShaderName() const
    {
        return m_data.Shader;
    }

    TextureList GetTexturesAssets()
    {
        return m_textureAssets;
    }

#pragma region CreateNew/Get
    static MaterialAsset* CreateNew( const std::string& name, const std::string& assetPath )
    {
        const auto asset = new MaterialAsset( name, assetPath );
        g_Assets->RegisterAsset( asset );
        return asset;
    }

    static MaterialAsset* Get( const std::string& name )
    {
        auto& assets = g_Assets->GetAssets( Type );
        const auto it = assets.find( name );

        if ( it == assets.end() )
        {
            CMD_ERROR( "Couldn't find material [" + name + "].", CMD::ASSETSYSTEM );
            return GetError();
        }

        const auto casted = dynamic_cast<MaterialAsset*>(it->second);
        return casted;
    }
#pragma endregion

    static MaterialAsset* m_error;

    static MaterialAsset* GetError()
    {
        assert( m_error != nullptr );
        return m_error;
    }
};

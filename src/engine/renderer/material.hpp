#pragma once

#include <optional>

#include "assets.hpp"
#include "texture.hpp"
#include "shader_program.hpp"

class Material
{
protected:
	std::shared_ptr<MaterialAsset> m_asset;

	std::optional<Texture> m_texture;
	ShaderProgram m_shader;

public:
	Material(std::shared_ptr<MaterialAsset> asset) : 
		m_asset(std::move(asset)),
		m_shader(m_asset->ShaderAsset().get())
	{
		auto texAsset = m_asset->TextureAsset();
		if(texAsset != nullptr)
			m_texture = Texture(texAsset.get());
	}

	~Material() { Dispose(); }

	Texture& GetTexture() { return m_texture.value(); }
	ShaderProgram& GetShader() { return m_shader; }

	void SetTexture(TextureAsset& texAsset)
	{
		if (!m_texture.has_value()) return;

		m_texture->Dispose();
		m_texture = Texture(&texAsset);
	}

	void SetTexture(std::string texName)
	{
		if (!m_texture.has_value()) return;

		auto asset = TextureAsset::Get(texName);
		if (asset != nullptr)
		{
			m_texture->Dispose();
			m_texture = Texture(asset.get());
		}
	}

	std::string GetName() { return m_asset->GetAssetPath().filename().string(); }
	std::filesystem::path GetPath() { return m_asset->GetAssetPath(); }
	
	// It can return nullptr.
	static std::shared_ptr<Material> Get(std::string matName)
	{
		auto asset = MaterialAsset::Get(matName);
		if (!asset)
		{
			CMD_LOG("Couldn't find material [" + matName + "]", WARNING);
			return nullptr;
		}

		return std::make_shared<Material>(std::move(asset));
	}

	static std::shared_ptr<Material> GetError()
	{
		return Get("error");
	}

	void Bind()
	{
		if (m_texture.has_value())
			m_texture->Bind();
		m_shader.Bind();
	}

	void Unbind()
	{
		if (m_texture.has_value())
			m_texture->Unbind();
		m_shader.Unbind();
	}

	void Dispose()
	{
		if (m_texture.has_value())
			m_texture->Dispose();
		m_shader.Dispose();
	}
};
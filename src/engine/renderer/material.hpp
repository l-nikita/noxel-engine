#pragma once

#include "assets.hpp"
#include "texture.hpp"
#include "shader_program.hpp"

class Material
{
protected:
	std::shared_ptr<MaterialAsset> m_asset;

	Texture m_texture;
	ShaderProgram m_shader;

public:
	Material(std::shared_ptr<MaterialAsset> asset) : 
		m_asset(std::move(asset)),
		m_texture(m_asset->TextureAsset().get()),
		m_shader(m_asset->ShaderAsset().get())
	{
	}

	~Material() { Dispose(); }

	Texture& GetTexture() { return m_texture; }
	ShaderProgram& GetShader() { return m_shader; }

	void SetTexture(TextureAsset& texAsset)
	{
		m_texture.Dispose();
		m_texture = Texture(&texAsset);
	}

	void SetTexture(std::string texName)
	{
		auto asset = TextureAsset::Get(texName);
		if (asset != nullptr)
		{
			m_texture.Dispose();
			m_texture = Texture(asset.get());
		}
	}
	
	// It can return nullptr.
	static std::shared_ptr<Material> Get(std::string matName)
	{
		auto asset = MaterialAsset::Get(matName);
		if (!asset) 
			return nullptr;

		return std::make_shared<Material>(std::move(asset));
	}

	static std::shared_ptr<Material> GetError()
	{
		return Get("Error");
	}

	void Bind()
	{
		m_texture.Bind();
		m_shader.Bind();
	}

	void Unbind()
	{
		m_texture.Unbind();
		m_shader.Unbind();
	}

	void Dispose()
	{
		m_texture.Dispose();
		m_shader.Dispose();
	}
};
#include <iostream>
#include <fstream>
#include <filesystem>

#include "asset_system.hpp"
#include "assets.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureAsset* TextureAsset::m_error = nullptr;
MaterialAsset* MaterialAsset::m_error = nullptr;

bool AssetSystem::Init()
{
	m_rootPath = std::filesystem::current_path().parent_path();
	m_binPath = std::filesystem::current_path().parent_path() / "bin";
	m_assetsPath = std::filesystem::current_path().parent_path() / "assets";

	ShaderAsset::CreateNew("default", (m_assetsPath / "shaders/default.hlsl").string());
	ShaderAsset::CreateNew("gizmos", (m_assetsPath / "shaders/gizmos.hlsl").string());

	TextureAsset::m_error = TextureAsset::CreateNew("error", (m_assetsPath / "materials/engine/error.png").string());
	MaterialAsset::m_error = MaterialAsset::CreateNew("error", (m_assetsPath / "materials/engine/error.nmt").string());

	MaterialAsset::CreateNew("gizmos", (m_assetsPath / "materials/engine/gizmos.nmt").string());

	MaterialAsset::CreateNew("dirt", (m_assetsPath / "materials/dirt.nmt").string());
	MaterialAsset::CreateNew("atlas", (m_assetsPath / "materials/atlas.nmt").string());

	CMD_LOG("] Asset system initialized", INFO);
	return true;
}

void AssetSystem::RegisterAsset(BaseAsset* asset)
{
	m_assets[asset->GetType()][asset->GetName()] = asset;
}
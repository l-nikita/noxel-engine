#include <iostream>
#include <fstream>
#include <filesystem>

#include "asset_system.hpp"
#include "shaderasset.hpp"
#include "textureasset.hpp"
#include "materialasset.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureAsset* TextureAsset::m_error = nullptr;
MaterialAsset* MaterialAsset::m_error = nullptr;

void AssetSystem::Init()
{
	m_rootPath = std::filesystem::current_path().parent_path();
	m_binPath = std::filesystem::current_path().parent_path() / "bin";
	m_assetsPath = std::filesystem::current_path().parent_path() / "assets";

	//TODO: Automatic assets searing
	//ShaderAsset::CreateNew("default", (m_assetsPath / "shaders\\default.hlsl").string());
	//ShaderAsset::CreateNew("gizmos", (m_assetsPath / "shaders\\gizmos.hlsl").string());
	ShaderAsset::CreateNew("geometrypass", (m_assetsPath / "shaders\\geometrypass").string());
	ShaderAsset::CreateNew("lightingpass", (m_assetsPath / "shaders\\lightingpass").string());
	ShaderAsset::CreateNew("skyboxpass", (m_assetsPath / "shaders\\skyboxpass").string());
	ShaderAsset::CreateNew("shadowspass", (m_assetsPath / "shaders\\shadowspass").string());

	TextureAsset::m_error = TextureAsset::CreateNew("error", (m_assetsPath / "materials\\engine\\error.png").string());
	MaterialAsset::m_error = MaterialAsset::CreateNew("error", (m_assetsPath / "materials\\engine\\error.nmt").string());
	//TODO: Make error shader
	//ShaderAsset::m_error = ;

	MaterialAsset::CreateNew("gizmos", (m_assetsPath / "materials\\engine\\gizmos.nmt").string());
	MaterialAsset::CreateNew("dirt1", (m_assetsPath / "materials\\dirt1.nmt").string());
	MaterialAsset::CreateNew("metal1", (m_assetsPath / "materials\\metal1.nmt").string());
	MaterialAsset::CreateNew("skybox", (m_assetsPath / "materials\\skybox.nmt").string());

	CMD_INFO("Asset system initialized", CMD::ASSETSYSTEM);
}

void AssetSystem::RegisterAsset(BaseAsset* asset)
{
	m_assets[asset->GetType()][asset->GetName()] = asset;
}
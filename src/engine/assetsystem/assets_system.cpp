#include <iostream>
#include <fstream>
#include <filesystem>

#include "assets_system.hpp"
#include "assets.hpp"
 
bool AssetsSystem::Init()
{
	m_rootPath = std::filesystem::current_path().parent_path();
	m_binPath = std::filesystem::current_path().parent_path() / "bin";
	m_assetsPath = std::filesystem::current_path().parent_path() / "assets";

	ShaderAsset::CreateNew("default", "E:/DEV/Sources/NoxelEngine/build/assets/shaders/default.glsl");
	MaterialAsset::CreateNew("error", "E:/DEV/Sources/NoxelEngine/build/assets/materials/engine/error.nmt");

	MaterialAsset::CreateNew("dirt", "E:/DEV/Sources/NoxelEngine/build/assets/materials/dirt.nmt");
	MaterialAsset::CreateNew("atlas", "E:/DEV/Sources/NoxelEngine/build/assets/materials/atlas.nmt");

	return true;
}

void AssetsSystem::RegisterAsset(AssetPtr asset)
{
	m_assets[asset->GetType()][asset->GetName()] = asset;
}
#include "assets_system.hpp"
#include "assets.hpp"
 
bool AssetsSystem::Init()
{
	ShaderAsset::CreateNew("default", "E:/DEV/Sources/NoxelEngine/build/assets/shaders/default.glsl");
	TextureAsset::CreateNew("dirt", "E:/DEV/Sources/NoxelEngine/build/assets/materials/dirt.png");
	return true;
}

void AssetsSystem::RegisterAsset(std::shared_ptr<BaseAsset> asset)
{
	m_assets[asset->GetType()][asset->GetName()] = asset;
}
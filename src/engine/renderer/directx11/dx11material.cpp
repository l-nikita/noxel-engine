#include <d3d11.h>

#include "dx11texture.hpp"
#include "dx11shaderprogram.hpp"
#include "dx11material.hpp"
#include "dx11factory.hpp"
#include "dx11renderer.hpp"
#include "assetsystem/materialasset.hpp"

DX11Material::DX11Material(ID3D11Device* device, ID3D11DeviceContext* context, MaterialAsset* asset)
    : m_device(device), m_context(context), m_asset(asset)
{
    auto factory = g_Renderer->GetFactory();
    if (asset->IsCubemap())
    {
        std::vector<void*> datas;
        int width = 0, height = 0;

        for (const auto& faceName : CUBEMAP_FACES_ORDER)
        {
            TextureAsset* faceAsset = nullptr;

            for (const auto& [paramName, asset] : m_asset->GetTexturesAssets())
            {
                if (paramName == faceName)
                    faceAsset = asset;
            }

            if (!faceAsset)
            {
                CMD_ERROR("Cubemap missing face: " + faceName, CMD::RENDERING);
                continue;
            }

            TextureData* data = faceAsset->Load();
            if (!data || !data->Image)
            {
                CMD_ERROR("Failed to load cubemap face: " + faceName, CMD::RENDERING);
                continue;
            }

            width = data->Width;
            height = data->Height;
            datas.push_back(data->Image);
        }

        if (datas.size() < CUBEMAP_FACES_COUNT)
        {
            CMD_ERROR("Failed to load cubemap.", CMD::RENDERING);
            return;
        }

        TextureDesc desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::RGBA8;
        desc.Usage = TextureUsage::SHADER_RESOURCE;
        desc.Data = datas;
        desc.IsCubemap = true;
        desc.MipLevels = 1;

        auto tex = factory->CreateTexture(desc);
        m_textures["cubemap"] = tex;

        for (auto data : datas)
            delete data;
    }
    else
    {
        for (const auto& [paramName, asset] : m_asset->GetTexturesAssets())
            m_textures[paramName] = factory->CreateTexture(asset);
    }

    auto shaderAsset = m_asset->GetShaderAsset();
    if (shaderAsset != nullptr)
        m_shader = factory->CreateShaderProgram(shaderAsset);
}

void DX11Material::Bind()
{
    for (const auto& [paramName, texture] : m_textures)
    {
        if (m_shader)
            m_shader->SetPSVar(paramName, texture);
    }

    if (m_shader) 
        m_shader->Bind();
}

void DX11Material::Unbind()
{
    if (m_shader)
        m_shader->Unbind();
}

void DX11Material::Release()
{
    auto factory = g_Renderer->GetFactory();
    for (const auto& [paramName, texture] : m_textures)
        factory->Release(texture);

    factory->Release(m_shader);
    m_textures.clear();
}

void DX11Material::SetShader(IShaderProgram* shader)
{
    m_shader = shader;
}

void DX11Material::SetTexture(std::string paramName, ITexture* texture)
{
    m_textures[paramName] = texture;
}

IShaderProgram* DX11Material::GetShader() const
{
    assert(m_shader != nullptr);
    return m_shader;
}

ITexture* DX11Material::GetTexture(std::string paramName)
{
    return m_textures[paramName];
}
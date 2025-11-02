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
        m_textures["cubemap"] = factory->CreateCubemap(asset);
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
    if (m_shader)
    {
        for (const auto& [paramName, texture] : m_textures)
        {
            if (texture)
                m_shader->SetPSVar(paramName, texture);
        }

        m_shader->Bind();
    }
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
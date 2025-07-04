#include "d3d11texture.hpp"
#include "d3d11shader.hpp"
#include "d3d11material.hpp"
#include <d3d11.h>

D3D11Material::D3D11Material(ID3D11Device* device, ID3D11DeviceContext* context, MaterialAsset* asset)
    : m_context(context), m_asset(asset)
{
    auto texAsset = m_asset->TextureAsset();
    if (texAsset != nullptr)
        m_textures[0] = new D3D11Texture(device, context, texAsset.get());

    auto shaderAsset = m_asset->ShaderAsset();
    if (shaderAsset != nullptr)
        m_shader = new D3D11Shader(device, context, shaderAsset.get());
}

void D3D11Material::Bind()
{
    if (m_shader)
        m_shader->Bind();

    for (unsigned int i = 0; i < MATERIAL_TEXTURES_SLOTS; ++i)
    {
        if (m_textures[i])
            m_textures[i]->Bind(i);
    }
}

void D3D11Material::Unbind()
{
    if (m_shader)
        m_shader->Unbind();

    for (unsigned int i = 0; i < MATERIAL_TEXTURES_SLOTS; ++i)
    {
        if (m_textures[i])
            m_textures[i]->Unbind(i);
    }
}

void D3D11Material::Destroy()
{
    delete m_shader;

    for (unsigned int i = 0; i < MATERIAL_TEXTURES_SLOTS; ++i)
    {
        if (m_textures[i])
            delete m_textures[i];
    }
}

void D3D11Material::SetShader(IShader* shader)
{
    m_shader = shader;
}

void D3D11Material::SetTexture(unsigned int slot, ITexture* texture)
{
    if (slot < MATERIAL_TEXTURES_SLOTS)
        m_textures[slot] = texture;
}

IShader* D3D11Material::GetShader() const
{
    return m_shader;
}

ITexture* D3D11Material::GetTexture(unsigned int slot) const
{
    if (slot < MATERIAL_TEXTURES_SLOTS)
        return m_textures[slot];

    return nullptr;
}
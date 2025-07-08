#include "dx11texture.hpp"
#include "dx11shaderprogram.hpp"
#include "dx11material.hpp"
#include "dx11factory.hpp"
#include "dx11renderer.hpp"
#include <d3d11.h>

DX11Material::DX11Material(ID3D11Device* device, ID3D11DeviceContext* context, MaterialAsset* asset)
    : m_context(context), m_asset(asset)
{
    auto texAsset = m_asset->TextureAsset();
    if (texAsset != nullptr)
        m_textures[0] = new DX11Texture(device, context, texAsset.get()); // TODO

    auto shaderAsset = m_asset->ShaderAsset();
    if (shaderAsset != nullptr)
        m_shader = g_Renderer->GetFactory()->CreateShaderProgram(shaderAsset.get());
}

void DX11Material::Bind()
{
    for (std::uint32_t i = 0; i < MATERIAL_TEXTURES_SLOTS; ++i)
    {
        auto* tex = m_textures[i];
        if (tex)
        {
            if (m_shader)
                m_shader->SetPSParameter("basetexture", tex);
        }
    }

    if (m_shader) m_shader->Bind();
}

void DX11Material::Unbind()
{
    if (m_shader)
        m_shader->Unbind();

    for (unsigned int i = 0; i < MATERIAL_TEXTURES_SLOTS; ++i)
    {
        if (m_textures[i])
            m_textures[i]->Unbind(i);
    }
}

void DX11Material::Destroy()
{
    delete m_shader;

    for (std::uint32_t i = 0; i < MATERIAL_TEXTURES_SLOTS; ++i)
    {
        if (m_textures[i])
            delete m_textures[i];
    }
}

void DX11Material::SetShader(IShaderProgram* shader)
{
    m_shader = shader;
}

void DX11Material::SetTexture(std::uint32_t slot, ITexture* texture)
{
    if (slot < MATERIAL_TEXTURES_SLOTS)
        m_textures[slot] = texture;
}

IShaderProgram* DX11Material::GetShader() const
{
    return m_shader;
}

ITexture* DX11Material::GetTexture(std::uint32_t slot) const
{
    if (slot < MATERIAL_TEXTURES_SLOTS)
        return m_textures[slot];

    return nullptr;
}
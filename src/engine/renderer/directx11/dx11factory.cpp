#pragma once

#include "dx11factory.hpp"
#include "dx11mesh.hpp"
#include "dx11material.hpp"
#include "dx11shaderprogram.hpp"
#include "dx11texture.hpp"
#include "dx11rendertexture.hpp"
#include "dx11depthstencil.hpp"
#include "dx11viewport.hpp"
#include "assetsystem/materialasset.hpp"

DX11Factory::DX11Factory(ID3D11Device* device, ID3D11DeviceContext* context) 
	: m_device(device), m_context(context)
{
}

IMesh* DX11Factory::CreateMesh(const MeshData& data)
{
    auto mesh = new DX11Mesh(
        m_device,
        m_context,
        data.Vertices.data(),
        sizeof(Vertex),
        static_cast<std::uint32_t>(std::size(data.Vertices)),
        data.Indices.data(),
        static_cast<std::uint32_t>(std::size(data.Indices))
    );

    m_resources.push_back(mesh);
    return mesh;
}

IShaderProgram* DX11Factory::CreateShaderProgram(ShaderAsset* asset)
{
    assert(asset);

    auto shader = new DX11ShaderProgram(m_device, m_context, asset);
    m_resources.push_back(shader);
    return shader;
}

ITexture* DX11Factory::CreateTexture(TextureAsset* asset)
{
    assert(asset);

    TextureData* data = asset->Load();

    TextureDesc desc = {};
    desc.Width = data->Width;
    desc.Height = data->Height;
    desc.Format = TextureFormat::RGBA8;
    desc.Usage = TextureUsage::SHADER_RESOURCE;
    desc.Data = { data->Image };
    desc.MipLevels = 1;

    auto tex = CreateTexture(desc);
    delete data;

    return tex;
}

ITexture* DX11Factory::CreateTexture(const TextureDesc& desc)
{
    auto tex = new DX11Texture(m_device, m_context, desc);
    m_resources.push_back(tex);
    return tex;
}

ITexture* DX11Factory::CreateCubemap(MaterialAsset* asset)
{
    if (!asset->IsCubemap())
    {
        CMD_ERROR("DX11 Factory: Failed to create cubemap because material [" + asset->GetName() + "] isn't cubemap.", CMD::RENDERING);
        return nullptr;
    }

    std::vector<void*> datas;
    int width = 0, height = 0;

    for (const auto& faceName : CUBEMAP_FACES_ORDER)
    {
        TextureAsset* faceAsset = nullptr;

        for (const auto& [paramName, asset] : asset->GetTexturesAssets())
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
        return nullptr;
    }

    TextureDesc desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = TextureFormat::RGBA8;
    desc.Usage = TextureUsage::SHADER_RESOURCE;
    desc.Data = datas;
    desc.IsCubemap = true;
    desc.MipLevels = 1;

    auto tex = CreateTexture(desc);
    for (auto data : datas)
        delete data;

    return tex;
}

IMaterial* DX11Factory::CreateMaterial(MaterialAsset* asset)
{
    assert(asset);

    auto mat = new DX11Material(m_device, m_context, asset);
    m_resources.push_back(mat);
    return mat;
}

IMaterial* DX11Factory::CreateMaterial(std::string assetName)
{
    assert(!assetName.empty());

    auto mat = new DX11Material(m_device, m_context, MaterialAsset::Get(assetName));
    m_resources.push_back(mat);
    return mat;
}

IRenderTexture* DX11Factory::CreateRenderTexture(int width, int height, bool depth, TextureFormat format)
{
    auto rt = new DX11RenderTexture(m_device, m_context, width, height, depth, format);
    m_resources.push_back(rt);
    return rt;
}

IDepthStencil* DX11Factory::CreateDepthStencil(int width, int height)
{
    auto depth = new DX11DepthStencil(m_device, m_context, width, height);
    m_resources.push_back(depth);
    return depth;
}

IViewport* DX11Factory::CreateViewport(int x, int y, int width, int height)
{
    return new DX11Viewport(x, y, width, height);
}

void DX11Factory::Release(IRenderResource* resource)
{
    if (!resource) 
        return;

    auto it = std::find(m_resources.begin(), m_resources.end(), resource);

    if (it != m_resources.end())
    {
        delete resource;
        m_resources.erase(it);
    }
}

void DX11Factory::Shutdown()
{
    for (int i = 0; i < m_resources.size(); i++)
        delete m_resources[i];

    m_resources.clear();
}
#pragma once

#include "dx11factory.hpp"
#include "dx11mesh.hpp"
#include "dx11material.hpp"
#include "dx11shaderprogram.hpp"

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
        static_cast<uint32_t>(std::size(data.Vertices)),
        data.Indices.data(),
        static_cast<uint32_t>(std::size(data.Indices))
    );

    return mesh;
}

IShaderProgram* DX11Factory::CreateShaderProgram(ShaderAsset* asset)
{
    assert(asset != nullptr);
    return new DX11ShaderProgram(m_device, m_context, asset);
}

IMaterial* DX11Factory::CreateMaterial(MaterialAsset* asset)
{
    assert(asset != nullptr);
    return new DX11Material(m_device, m_context, asset);
}

IMaterial* DX11Factory::CreateMaterial(std::string assetName)
{
    assert(!assetName.empty());
    return new DX11Material(m_device, m_context, MaterialAsset::Get(assetName));
}
#include "d3d11buffer.hpp"
#include "d3d11mesh.hpp"
#include <stdexcept>
#include <cstdint>
#include <d3d11.h>

D3D11Mesh::D3D11Mesh(ID3D11Device* device, ID3D11DeviceContext* context,
    const void* vertices, uint32_t vertexSize, uint32_t vertexCount,
    const uint32_t* indices, uint32_t indexCount)
    : m_context(context), m_vertexCount(vertexCount), m_indexCount(indexCount), m_vertexStride(vertexSize)
{
    m_vertexBuffer = new D3D11Buffer(
        device,
        context,
        GPUBufferType::VERTEX,
        vertexSize * vertexCount,
        vertices);

    m_indexBuffer = new D3D11Buffer(
        device,
        context,
        GPUBufferType::INDEX,
        sizeof(uint32_t) * indexCount,
        indices);
}

void D3D11Mesh::Destroy()
{
    delete m_material;
    delete m_vertexBuffer;
    delete m_indexBuffer;
}

void D3D11Mesh::Bind() 
{
    if (m_vertexBuffer) m_vertexBuffer->Bind(0);
    if (m_indexBuffer) m_indexBuffer->Bind(0);

    if (m_material)
        m_material->Bind();
}

void D3D11Mesh::Unbind()
{
    if (m_vertexBuffer) m_vertexBuffer->Unbind(0);
    if (m_indexBuffer) m_indexBuffer->Unbind(0);

    if (m_material)
        m_material->Unbind();
}

void D3D11Mesh::SetMaterial(IMaterial* material) 
{
    m_material = material;
}

IMaterial* D3D11Mesh::GetMaterial() const 
{
    return m_material;
}

void D3D11Mesh::Draw() 
{
    m_material->Bind();
    m_context->DrawIndexed(m_indexCount, 0, 0);
    m_material->Unbind();
}
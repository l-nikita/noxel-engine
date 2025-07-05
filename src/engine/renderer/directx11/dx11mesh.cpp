#include "dx11buffer.hpp"
#include "dx11mesh.hpp"
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <d3d11.h>

DX11Mesh::DX11Mesh(ID3D11Device* device, ID3D11DeviceContext* context,
    const void* vertices, uint32_t vertexSize, uint32_t vertexCount,
    const uint32_t* indices, uint32_t indexCount)
    : m_context(context), m_vertexCount(vertexCount), m_indexCount(indexCount), m_vertexStride(vertexSize)
{
    m_vertexBuffer = new DX11Buffer(
        device,
        context,
        GPUBufferType::VERTEX,
        vertexSize * vertexCount,
        vertices, 
        vertexSize);

    m_indexBuffer = new DX11Buffer(
        device,
        context,
        GPUBufferType::INDEX,
        sizeof(uint32_t) * indexCount,
        indices);
}

void DX11Mesh::Destroy()
{
    delete m_vertexBuffer;
    delete m_indexBuffer;
}

void DX11Mesh::Bind()
{
    if (m_vertexBuffer) m_vertexBuffer->Bind(0);
    if (m_indexBuffer) m_indexBuffer->Bind(0);
}

void DX11Mesh::Unbind()
{
    if (m_vertexBuffer) m_vertexBuffer->Unbind(0);
    if (m_indexBuffer) m_indexBuffer->Unbind(0);
}

void DX11Mesh::Draw()
{
    Bind();
    m_context->DrawIndexed(m_indexCount, 0, 0);
    Unbind();
}
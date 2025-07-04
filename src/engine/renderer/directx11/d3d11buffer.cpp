
#include <d3d11.h>

#include "d3d11buffer.hpp"
#include "renderer/vertex.hpp"

D3D11Buffer::D3D11Buffer(ID3D11Device* device, ID3D11DeviceContext* context, GPUBufferType type, size_t size, const void* initialData)
    : m_device(device), m_context(context), m_type(type)
{
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(size);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = GetBindFlags(type);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = initialData;

    device->CreateBuffer(&desc, initialData ? &data : nullptr, &m_buffer);
}

void D3D11Buffer::Bind(unsigned int slot = 0) 
{
    switch (m_type) 
    {
        case GPUBufferType::VERTEX:
        {
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            m_context->IASetVertexBuffers(slot, 1, &m_buffer, &stride, &offset);
            break;
        }
        case GPUBufferType::INDEX:
            m_context->IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
            break;
        case GPUBufferType::VS_CONSTANT:
            m_context->VSSetConstantBuffers(slot, 1, &m_buffer);
            break;
        case GPUBufferType::PS_CONSTANT:
            m_context->PSSetConstantBuffers(slot, 1, &m_buffer);
            break;
    }
}

void D3D11Buffer::Unbind(unsigned int slot = 0)
{
    switch (m_type)
    {
        case GPUBufferType::VERTEX:
        {
            UINT zeroStride = 0;
            UINT zeroOffset = 0;
            ID3D11Buffer* nullBuffer[1] = { nullptr };
            m_context->IASetVertexBuffers(slot, 1, nullBuffer, &zeroStride, &zeroOffset);
            break;
        }
        case GPUBufferType::INDEX:
            m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
            break;
        case GPUBufferType::VS_CONSTANT:
            ID3D11Buffer* nullBuffer[1] = { nullptr };
            m_context->VSSetConstantBuffers(slot, 1, nullBuffer);
            break;
        case GPUBufferType::PS_CONSTANT:
            ID3D11Buffer* nullBuffer[1] = { nullptr };
            m_context->PSSetConstantBuffers(slot, 1, nullBuffer);
            break;
    }
}

void D3D11Buffer::Update(const void* data, size_t size)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    m_context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, data, size);
    m_context->Unmap(m_buffer, 0);
}

void D3D11Buffer::Destroy()
{
    if (m_buffer) 
        m_buffer->Release();

    m_buffer = nullptr;
}

unsigned int D3D11Buffer::GetBindFlags(GPUBufferType type)
{
    switch (type) 
    {
        case GPUBufferType::VERTEX: return D3D11_BIND_VERTEX_BUFFER;
        case GPUBufferType::INDEX: return D3D11_BIND_INDEX_BUFFER;
        case GPUBufferType::PS_CONSTANT: return D3D11_BIND_CONSTANT_BUFFER;
        case GPUBufferType::VS_CONSTANT: return D3D11_BIND_CONSTANT_BUFFER;
    }

    return 0;
}
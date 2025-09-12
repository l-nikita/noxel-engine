#include <d3d11.h>
#include "dx11buffer.hpp"
#include "renderer/vertex.hpp"

DX11Buffer::DX11Buffer(ID3D11Device* device, ID3D11DeviceContext* context, GPUBufferType type, size_t size, const void* initialData, std::uint32_t stride)
    : m_device(device), m_context(context), m_type(type), m_stride(stride)
{
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(size);
    desc.BindFlags = GetBindFlags(type);
    desc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = initialData;

    device->CreateBuffer(&desc, initialData ? &data : nullptr, &m_buffer);
}

void DX11Buffer::Bind(std::uint32_t slot)
{
    switch (m_type)
    {
        case GPUBufferType::VERTEX:
        {
            UINT offset = 0;
            m_context->IASetVertexBuffers(slot, 1, &m_buffer, &m_stride, &offset);
            break;
        }
        case GPUBufferType::INDEX:
        {
            m_context->IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
            break;
        }
        case GPUBufferType::VS_CONSTANT:
        {
            m_context->VSSetConstantBuffers(slot, 1, &m_buffer);
            break;
        }
        case GPUBufferType::PS_CONSTANT:
        {
            m_context->PSSetConstantBuffers(slot, 1, &m_buffer);
            break;
        }
    }
}

void DX11Buffer::Unbind(std::uint32_t slot)
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
        {
            m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
            break;
        }
        case GPUBufferType::VS_CONSTANT:
        {
            ID3D11Buffer* nullBuffer[1] = { nullptr };
            m_context->VSSetConstantBuffers(slot, 1, nullBuffer);
            break;
        }
        case GPUBufferType::PS_CONSTANT:
        {
            ID3D11Buffer* nullBuffer[1] = { nullptr };
            m_context->PSSetConstantBuffers(slot, 1, nullBuffer);
            break;
        }
    }
}

void DX11Buffer::Update(const void* data, size_t size)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    m_context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, data, size);
    m_context->Unmap(m_buffer, 0);
}

void DX11Buffer::Destroy()
{
    if (m_buffer)
        m_buffer->Release();

    m_buffer = nullptr;
}

std::uint32_t DX11Buffer::GetBindFlags(GPUBufferType type)
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
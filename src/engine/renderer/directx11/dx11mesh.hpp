#pragma once

#include <d3d11.h>
#include "renderer/imesh.hpp"
#include "renderer/imaterial.hpp"
#include <cstdint>

class DX11Buffer;

class DX11Mesh : public IMesh 
{
public:
    DX11Mesh(ID3D11Device* device, ID3D11DeviceContext* context,
        const void* vertices, uint32_t vertexSize, uint32_t vertexCount,
        const void* indices, uint32_t indexCount);

    ~DX11Mesh() { Destroy(); }

    void Bind() override;
    void Unbind() override;
    void Draw() override;
    void Destroy() override;

private:
    ID3D11DeviceContext* m_context;
    DX11Buffer* m_vertexBuffer = nullptr;
    DX11Buffer* m_indexBuffer = nullptr;

    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
    uint32_t m_vertexStride = 0;
};
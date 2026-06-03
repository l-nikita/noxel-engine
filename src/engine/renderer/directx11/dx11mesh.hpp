#pragma once

#include <d3d11.h>
#include "renderer/imesh.hpp"
#include <cstdint>

class DX11Buffer;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11Mesh : public IMesh
{
public:
    DX11Mesh( ID3D11Device* device, ID3D11DeviceContext* context,
              const void* vertices, std::uint32_t vertexSize, std::uint32_t vertexCount,
              const void* indices, std::uint32_t indexCount );

    ~DX11Mesh()
    {
        Release();
    }

    void Bind() override;
    void Unbind() override;
    void Draw() override;
    void Release() override;

private:
    DX11Buffer* m_vertexBuffer = nullptr;
    DX11Buffer* m_indexBuffer = nullptr;

    std::uint32_t m_vertexCount = 0;
    std::uint32_t m_indexCount = 0;
    std::uint32_t m_vertexStride = 0;

private:
    ID3D11DeviceContext* m_context;
};
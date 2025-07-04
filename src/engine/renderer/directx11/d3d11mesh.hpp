#pragma once

#include "renderer/imesh.hpp"
#include "renderer/imaterial.hpp"
#include <cstdint>

class D3D11Mesh : public IMesh 
{
public:
    D3D11Mesh(ID3D11Device* device, ID3D11DeviceContext* context,
        const void* vertices, uint32_t vertexSize, uint32_t vertexCount,
        const uint32_t* indices, uint32_t indexCount);

    ~D3D11Mesh() { Destroy(); }

    void Bind() override;
    void Unbind() override;
    void Draw() override;
    void Destroy() override;

    void SetMaterial(IMaterial* material) override;
    IMaterial* GetMaterial() const override;

private:
    ID3D11DeviceContext* m_context;
    D3D11Buffer* m_vertexBuffer = nullptr;
    D3D11Buffer* m_indexBuffer = nullptr;

    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
    uint32_t m_vertexStride = 0;

    IMaterial* m_material = nullptr;
};

struct ID3D11Device;
struct ID3D11DeviceContext;
struct D3D11Buffer;
#pragma once

#include "renderer/igpubuffer.hpp"
#include <cstdint>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

class D3D11Buffer : public IGPUBuffer 
{
public:
    D3D11Buffer(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        GPUBufferType type,
        size_t size,
        const void* initialData = nullptr);

    void Bind(uint32_t slot = 0) override;
    void Unbind(uint32_t slot = 0) override;
    void Update(const void* data, size_t size) override;
    void Destroy() override;

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    ID3D11Buffer* m_buffer = nullptr;
    GPUBufferType m_type;

    uint32_t GetBindFlags(GPUBufferType type);
};
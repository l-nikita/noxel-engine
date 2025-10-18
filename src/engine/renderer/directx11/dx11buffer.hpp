#pragma once

#include <d3d11.h>
#include <cstdint>
#include "renderer/igpubuffer.hpp"

class DX11Buffer : public IGPUBuffer
{
public:
    DX11Buffer(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        GPUBufferType type,
        size_t size,
        const void* initialData = nullptr, 
        std::uint32_t stride = 0);

    ~DX11Buffer() { Release(); }

    void Bind(std::uint32_t slot = 0) override;
    void Unbind(std::uint32_t slot = 0) override;
    void Update(const void* data, size_t size) override;
    void Release() override;

private:
    UINT m_stride = 0;
    ID3D11Buffer* m_buffer = nullptr;
    GPUBufferType m_type;

    std::uint32_t GetBindFlags(GPUBufferType type);

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
};
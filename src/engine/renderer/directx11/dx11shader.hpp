#pragma once

#include <d3d11.h>
#include "renderer/ishader.hpp"
#include "assetsystem/assets.hpp"

class DX11Shader : public IShader 
{
public:
    DX11Shader(
        ID3D11Device* device, 
        ID3D11DeviceContext* context, 
        ShaderAsset* asset);

    ~DX11Shader() { Destroy(); }

    void Bind() override;
    void Unbind() override;
    void Destroy() override;

    ID3D10Blob* GetVSBlob() const;
    ID3D10Blob* GetPSBlob() const;

private:
    void Compile(ShaderAsset* asset, ShaderType type);

    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    ID3D11InputLayout* m_inputLayout;
    ID3D11VertexShader* m_vs;
    ID3D11PixelShader* m_ps;
    ID3D10Blob* m_vsBlob;
    ID3D10Blob* m_psBlob;
};
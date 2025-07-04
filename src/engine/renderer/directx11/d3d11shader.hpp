#pragma once

#include "renderer/ishader.hpp"
#include "assetsystem/assets.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3DBlob;

class D3D11Shader : public IShader 
{
public:
    D3D11Shader(
        ID3D11Device* device, 
        ID3D11DeviceContext* context, 
        ShaderAsset* asset);

    ~D3D11Shader() { Destroy(); }

    void Bind() override;
    void Unbind() override;
    void Destroy() override;

    ID3DBlob* GetVSBlob() const { return m_vsBlob; }
    ID3DBlob* GetPSBlob() const { return m_psBlob; }

private:
    void Compile(ShaderAsset* asset, ShaderType type);

    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    ID3D11VertexShader* m_vs;
    ID3D11PixelShader* m_ps;
    ID3DBlob* m_vsBlob;
    ID3DBlob* m_psBlob;
};
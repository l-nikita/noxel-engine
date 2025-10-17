#pragma once

#include <d3d11.h>
#include <map>
#include "renderer/ishaderprogram.hpp"
#include "assetsystem/assets.hpp"
#include <glm/gtc/type_ptr.hpp>

class DX11ShaderProgram : public IShaderProgram
{
public:
    DX11ShaderProgram(
        ID3D11Device* device, 
        ID3D11DeviceContext* context, 
        ShaderAsset* asset);

    ~DX11ShaderProgram() { Destroy(); }

    void Bind() override;
    void Unbind() override;
    void Destroy() override;

    ID3D10Blob* GetVSBlob() const;
    ID3D10Blob* GetPSBlob() const;

    IShader* GetPSShader() { return m_pShader; }
    IShader* GetVSShader() { return m_vShader; }

    ID3D11Device* GetDevice() { return m_device; };
    ID3D11DeviceContext* GetContext() { return m_context; };

    void SetPSParameter(const std::string& name, const MaterialValue& value) override { GetPSShader()->SetParameter(name, value); }
    void SetVSParameter(const std::string& name, const MaterialValue& value) override { GetVSShader()->SetParameter(name, value); }

private:
    void Compile(ShaderAsset* asset, ShaderType type);

private:
    IShader* m_pShader;
    IShader* m_vShader;

    ID3D11VertexShader* m_vs;
    ID3D11PixelShader* m_ps;
    ID3D10Blob* m_vsBlob;
    ID3D10Blob* m_psBlob;

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
};
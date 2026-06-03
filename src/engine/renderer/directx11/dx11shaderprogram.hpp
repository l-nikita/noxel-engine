#pragma once

#include <d3d11.h>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "renderer/ishaderprogram.hpp"

class ShaderAsset;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11ShaderProgram : public IShaderProgram
{
public:
    DX11ShaderProgram(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        ShaderAsset* asset );

    ~DX11ShaderProgram()
    {
        Release();
    }

    void Bind() override;
    void Unbind() override;
    void Release() override;

    ID3D10Blob* GetVSBlob() const;
    ID3D10Blob* GetPSBlob() const;

    IShader* GetPSShader()
    {
        return m_pShader;
    }

    IShader* GetVSShader()
    {
        return m_vShader;
    }

    ID3D11Device* GetDevice()
    {
        return m_device;
    };

    ID3D11DeviceContext* GetContext()
    {
        return m_context;
    };

    void SetPSVar( const std::string& name, const ShaderVar& var ) override
    {
        GetPSShader()->SetVar(name, var);
    }

    void SetVSVar( const std::string& name, const ShaderVar& var ) override
    {
        GetVSShader()->SetVar(name, var);
    }

private:
    void Compile( ShaderAsset* asset, ShaderType type );

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
#pragma once

#include <d3d11.h>
#include "filesystem/ifilesystem.hpp"
#include "renderer/ishaderprogram.hpp"

struct ShaderData;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11ShaderProgram : public IShaderProgram
{
public:
    DX11ShaderProgram(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        ShaderData* data );

    ~DX11ShaderProgram() override
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
        GetPSShader()->SetVar( name, var );
    }

    void SetVSVar( const std::string& name, const ShaderVar& var ) override
    {
        GetVSShader()->SetVar( name, var );
    }

private:
    void Compile( const FPath& path, ShaderType type, const std::string& shaderName );

private:
    IShader* m_pShader = nullptr;
    IShader* m_vShader = nullptr;

    ID3D11VertexShader* m_vs = nullptr;
    ID3D11PixelShader* m_ps = nullptr;
    ID3D10Blob* m_vsBlob = nullptr;
    ID3D10Blob* m_psBlob = nullptr;

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
};

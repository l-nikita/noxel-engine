#pragma once

#include <d3d11.h>
#include "filesystem/ifilesystem.hpp"
#include "renderer/ishaderprogram.hpp"

struct ShaderData;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11ShaderProgram final : public IShaderProgram
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

    ID3DBlob* GetVSBlob() const;
    ID3DBlob* GetPSBlob() const;

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
    void Compile( const FPath& path, ShaderType type, const std::string& name );

private:
    IShader* m_pShader = nullptr;
    IShader* m_vShader = nullptr;

    ID3D11VertexShader* m_vs = nullptr;
    ID3D11PixelShader* m_ps = nullptr;
    ID3DBlob* m_vsBlob = nullptr;
    ID3DBlob* m_psBlob = nullptr;

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11ShaderInclude final : public ID3DInclude
{
public:
    explicit DX11ShaderInclude( const FPath& shaderPath )
        : m_shaderDir( shaderPath.parent_path() )
    {
    }

    STDMETHOD( Open )( D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData,
                       UINT* pBytes ) override
    {
        FPath includePath = m_shaderDir / pFileName;

        auto fileData = g_FileSystem->ReadText( includePath );
        if ( fileData.empty() )
            return E_FAIL;

        char* buffer = new char[ fileData.size() ];
        std::memcpy( buffer, fileData.data(), fileData.size() );

        *ppData = buffer;
        *pBytes = static_cast<UINT>(fileData.size());

        return S_OK;
    }

    STDMETHOD( Close )( LPCVOID pData ) override
    {
        char* buffer = (char*)pData;
        delete[] buffer;
        return S_OK;
    }

private:
    FPath m_shaderDir;
};

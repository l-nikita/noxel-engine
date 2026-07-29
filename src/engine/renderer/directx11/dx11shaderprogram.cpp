#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

#include "dbg/debug.hpp"
#include "dx11shaderprogram.hpp"
#include "renderer/vertex.hpp"
#include "renderer/shaderregistry.hpp"
#include "assetsystem/asset.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/shaderdata.hpp"

DX11ShaderProgram::DX11ShaderProgram(
    ID3D11Device* device, ID3D11DeviceContext* context, const Asset* asset )
    : m_device( device ), m_context( context ), m_vs( nullptr ), m_ps( nullptr ), m_vsBlob( nullptr ),
      m_psBlob( nullptr )
{
    assert( asset );

    std::unique_ptr<ShaderData> data( g_Assets->LoadAsset<ShaderData>( asset ) );
    if ( !data )
        FATAL_ERROR( "Failed to load shader asset [" + asset->Name + "]", CMD::RENDERING );

    if ( !ShaderRegistry::IsShaderExist( data->Name ) )
        FATAL_ERROR( "Shader [" + data->Name + "] not found in registry.", CMD::RENDERING );

    Compile( g_FileSystem->Absolute( data->VertexShaderPath ), VERTEX, data->Name );
    Compile( g_FileSystem->Absolute( data->PixelShaderPath ), PIXEL, data->Name );

    HRESULT hr = m_device->CreateVertexShader(
        m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(),
        nullptr, &m_vs );

    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: Failed to create vertex dx11 shader", CMD::RENDERING );

    hr = m_device->CreatePixelShader(
        m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(),
        nullptr, &m_ps );

    if ( FAILED( hr ) )
        FATAL_ERROR( "DX11: Failed to create pixel dx11 shader", CMD::RENDERING );

    m_vShader = ShaderRegistry::Create( data->Name + "VS", this );
    if ( !m_vShader )
        FATAL_ERROR( "DX11: Failed to create pixel shader", CMD::RENDERING );

    m_pShader = ShaderRegistry::Create( data->Name + "PS", this );
    if ( !m_pShader )
        FATAL_ERROR( "DX11: Failed to create pixel shader", CMD::RENDERING );
}

void DX11ShaderProgram::Compile( const FPath& path, ShaderType type, const std::string& shaderName )
{
    ID3D10Blob* errorBlob = nullptr;
    HRESULT hr = E_FAIL;

    uint32_t compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    ID3DInclude* includeHandler = D3D_COMPILE_STANDARD_FILE_INCLUDE;
#ifdef _DEBUG
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    if ( type == VERTEX )
    {
        hr = D3DCompileFromFile(
            path.c_str(),
            nullptr, // defines
            includeHandler, // include handler
            "main", // entry point
            "vs_5_0", // shader model
            compileFlags,
            0,
            &m_vsBlob,
            &errorBlob
        );
    } else if ( type == PIXEL )
    {
        hr = D3DCompileFromFile(
            path.c_str(),
            nullptr,
            includeHandler,
            "main",
            "ps_5_0",
            compileFlags,
            0,
            &m_psBlob,
            &errorBlob
        );
    } else
        FATAL_ERROR( "DX11: Unknown shader type!", CMD::RENDERING );

    if ( FAILED( hr ) )
    {
        if ( errorBlob )
        {
            OutputDebugStringA( static_cast<char*>(errorBlob->GetBufferPointer()) );
            CMD_ERROR(
                "DX11: Shader compilation error: " + std::string( static_cast<char*>(errorBlob->GetBufferPointer()) ),
                CMD::RENDERING );
            errorBlob->Release();
        }

        if ( type == VERTEX )
            FATAL_ERROR( "DX11: Failed to compile vertex shader! [" + shaderName + "]", CMD::RENDERING );
        else if ( type == PIXEL )
            FATAL_ERROR( "DX11: Failed to compile pixel shader! [" + shaderName + "]", CMD::RENDERING );
    }
}

void DX11ShaderProgram::Bind()
{
    if ( m_vShader ) m_vShader->Bind();
    if ( m_pShader ) m_pShader->Bind();
    m_context->VSSetShader( m_vs, nullptr, 0 );
    m_context->PSSetShader( m_ps, nullptr, 0 );
}

void DX11ShaderProgram::Unbind()
{
    if ( m_vShader ) m_vShader->Unbind();
    if ( m_pShader ) m_pShader->Unbind();
    m_context->VSSetShader( nullptr, nullptr, 0 );
    m_context->PSSetShader( nullptr, nullptr, 0 );
}

void DX11ShaderProgram::Release()
{
    if ( IsReleased() )
        return;

    m_isReleased = true;

    if ( m_vShader )
        delete m_vShader, m_vShader = nullptr;

    if ( m_pShader )
        delete m_pShader, m_pShader = nullptr;

    if ( m_vs )
        m_vs->Release(), m_vs = nullptr;

    if ( m_ps )
        m_ps->Release(), m_ps = nullptr;

    if ( m_vsBlob )
        m_vsBlob->Release(), m_vsBlob = nullptr;

    if ( m_psBlob )
        m_psBlob->Release(), m_psBlob = nullptr;
}

ID3DBlob* DX11ShaderProgram::GetVSBlob() const
{
    assert( m_vsBlob );
    return m_vsBlob;
}

ID3DBlob* DX11ShaderProgram::GetPSBlob() const
{
    assert( m_psBlob );
    return m_psBlob;
}

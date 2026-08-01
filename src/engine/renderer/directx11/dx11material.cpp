#include <d3d11.h>

#include "dx11material.hpp"
#include "dx11factory.hpp"
#include "dx11renderer.hpp"
#include "dx11shaderprogram.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/materialdata.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DX11Material::DX11Material( ID3D11Device* device, ID3D11DeviceContext* context, MaterialData* data )
    : m_context( context ), m_device( device )
{
    assert( data );

    auto factory = g_Renderer->GetFactory();
    m_shader = factory->CreateShaderProgram( data->Shader );

    if ( data->IsCubemap )
    {
        m_textures[ "cubemap" ] = factory->CreateCubemap( data->Id );
    } else
    {
        for ( const auto& [ paramName, paramVal ]: data->Parameters )
        {
            if ( const auto* id = std::get_if<AssetId>( &paramVal ) )
            {
                const auto ass = g_Assets->GetAsset( *id );
                if ( ass->Type == AssetType::TEXTURE )
                    m_textures[ paramName ] = factory->CreateTexture( ass->Id );
            }
        }
    }

    assert( m_shader );
}

void DX11Material::Bind()
{
    if ( m_shader )
    {
        for ( const auto& [ paramName, texture ]: m_textures )
        {
            if ( texture )
                m_shader->SetPSVar( paramName, texture );
        }

        m_shader->Bind();
    }
}

void DX11Material::Unbind()
{
    if ( m_shader ) m_shader->Unbind();
}

void DX11Material::Release()
{
    if ( IsReleased() )
        return;

    m_isReleased = true;

    auto factory = g_Renderer->GetFactory();
    if ( m_shader )
        factory->Release( m_shader ), m_shader = nullptr;

    for ( const auto& texture: m_textures | std::views::values )
        if ( texture )
            factory->Release( texture );

    m_textures.clear();
}

void DX11Material::SetShader( IShaderProgram* shader )
{
    m_shader = shader;
}

void DX11Material::SetTexture( std::string paramName, ITexture* texture )
{
    m_textures[ paramName ] = texture;
}

IShaderProgram* DX11Material::GetShader() const
{
    assert( m_shader );
    return m_shader;
}

ITexture* DX11Material::GetTexture( std::string paramName )
{
    return m_textures[ paramName ];
}

#include <d3d11.h>

#include "dx11material.hpp"
#include "dx11factory.hpp"
#include "dx11renderer.hpp"
#include "dx11shaderprogram.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/materialdata.hpp"
#include "dbg/debug.hpp"

DX11Material::DX11Material( ID3D11Device* device, ID3D11DeviceContext* context, const Asset* asset )
    : m_device( device ), m_context( context )
{
    assert( asset );

    auto factory = g_Renderer->GetFactory();
    std::unique_ptr<MaterialData> data( g_Assets->LoadAsset<MaterialData>( asset ) );
    if ( !data )
        return;

    const auto shaderAsset = g_Assets->GetAsset( data->Shader, AssetType::SHADER );
    if ( shaderAsset )
        m_shader = factory->CreateShaderProgram( shaderAsset );
    else
        CMD_ERROR( "Shader asset not found: " + data->Shader, CMD::RENDERING );

    if ( data->IsCubemap )
        m_textures[ "cubemap" ] = factory->CreateCubemap( asset );
    else
    {
        for ( const auto& [ paramName, parameter ]: data->Parameters.items() )
        {
            if ( !parameter.is_string() )
                continue;

            const auto textureReference = parameter.get<std::string>();
            const auto texturePath = asset->Path.parent_path() / textureReference;

            auto texAsset = g_Assets->GetAsset( texturePath, AssetType::TEXTURE );
            if ( !texAsset )
                texAsset = g_Assets->GetAsset( textureReference, AssetType::TEXTURE );

            if ( !texAsset )
            {
                CMD_ERROR( "Texture asset not found: " + texturePath.string(), CMD::RENDERING );
                texAsset = g_Assets->GetAsset( "error", AssetType::TEXTURE );
            }

            if ( texAsset )
                m_textures[ paramName ] = factory->CreateTexture( texAsset );
        }
    }
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
    assert( m_shader != nullptr );
    return m_shader;
}

ITexture* DX11Material::GetTexture( std::string paramName )
{
    return m_textures[ paramName ];
}

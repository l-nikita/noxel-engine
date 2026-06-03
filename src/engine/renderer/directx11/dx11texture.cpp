#include <stdexcept>
#include <string>
#include <d3d11.h>
#include "dx11texture.hpp"
#include "dx11helper.hpp"
#include "assetsystem/textureasset.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DX11Texture::DX11Texture( ID3D11Device* device, ID3D11DeviceContext* context, const TextureDesc& desc )
    : m_device(device), m_context(context)
{
    D3D11_TEXTURE2D_DESC texDesc = DX11Helper::CreateTexture2DDesc(desc);

    if ( desc.Data.size() > 0 )
    {
        std::vector<D3D11_SUBRESOURCE_DATA> res(desc.Data.size());
        for ( size_t i = 0; i < desc.Data.size(); i++ )
        {
            res[ i ].pSysMem = desc.Data[ i ];
            res[ i ].SysMemPitch = desc.Width * 4; // RGBA8
            res[ i ].SysMemSlicePitch = 0;
        }

        DX11Helper::CreateTexture2D(device, &texDesc, res.data(), &m_texture);
    } else
        DX11Helper::CreateTexture2D(device, &texDesc, nullptr, &m_texture);

    D3D11_SRV_DIMENSION srvDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    if ( desc.IsCubemap )
        srvDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

    DX11Helper::CreateShaderResourceViewForTexture2D(device, m_texture, &m_srv,
                                                     DX11Helper::ToDXGIFormat(desc.ShaderFormat), srvDimension);

    m_width = desc.Width;
    m_height = desc.Height;
    m_format = desc.Format;
    m_isCubemap = desc.IsCubemap;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DX11Texture::DX11Texture( ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture )
    : m_device(device), m_context(context), m_texture(texture)
{
    D3D11_TEXTURE2D_DESC desc;
    m_texture->GetDesc(&desc);

    DX11Helper::CreateShaderResourceViewForTexture2D(device, m_texture, &m_srv, desc.Format);

    m_width = desc.Width;
    m_height = desc.Height;
    m_format = DX11Helper::ToTextureFormat(desc.Format);
    m_isCubemap = desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE;
}

void DX11Texture::Release()
{
    if ( IsReleased() )
        return;

    m_isReleased = true;

    if ( m_srv )
        m_srv->Release(), m_srv = nullptr;

    if ( m_texture )
        m_texture->Release(), m_texture = nullptr;
}

void DX11Texture::Bind( std::uint32_t slot )
{
    assert(m_srv);
    m_context->PSSetShaderResources(slot, 1, &m_srv);
}

void DX11Texture::Unbind( std::uint32_t slot )
{
    ID3D11ShaderResourceView* nullSRV[ 1 ] = {nullptr};
    m_context->PSSetShaderResources(slot, 1, nullSRV);
}

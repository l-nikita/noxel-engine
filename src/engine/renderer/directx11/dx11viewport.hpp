#pragma once

#include <d3d11.h>
#include "renderer/iviewport.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct DX11Viewport : IViewport
{
    D3D11_VIEWPORT vp{};

    DX11Viewport( float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f )
        : IViewport( x, y, width, height, minDepth, maxDepth )
    {
        vp.TopLeftX = X;
        vp.TopLeftY = Y;
        vp.Width = Width;
        vp.Height = Height;
        vp.MinDepth = MinDepth;
        vp.MaxDepth = MaxDepth;
    }

    void Resize( float width, float height ) override
    {
        IViewport::Resize( width, height );
        vp.Width = width;
        vp.Height = height;
    }

    void Apply( ID3D11DeviceContext* context ) const
    {
        context->RSSetViewports( 1, &vp );
    }

    void Release() override
    {
    }
};

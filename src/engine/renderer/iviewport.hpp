#pragma once

#include "renderer/irenderresource.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct IViewport : IRenderResource
{
    float X = 0;
    float Y = 0;
    float Width = 0;
    float Height = 0;
    float MinDepth = 0.0f;
    float MaxDepth = 1.0f;

    IViewport( float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f )
        : X( x ), Y( y ), Width( width ), Height( height ), MinDepth( minDepth ), MaxDepth( maxDepth )
    {
    }

    virtual void Resize( float width, float height )
    {
        Width = width;
        Height = height;
    }
};

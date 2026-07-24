#pragma once

#include <stb_image.h>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct TextureData
{
    unsigned char* Image = nullptr;
    int Width = 0;
    int Height = 0;
    int Channels = 0;

    TextureData() = default;

    ~TextureData()
    {
        Release();
    }

    // disable copying
    TextureData( const TextureData& ) = delete;
    TextureData& operator=( const TextureData& ) = delete;

    void Release()
    {
        if ( Image )
        {
            stbi_image_free( Image );
            Image = nullptr;
            Width = 0;
            Height = 0;
            Channels = 0;
        }
    }
};

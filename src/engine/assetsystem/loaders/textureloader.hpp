#pragma once

#include <stb_image.h>

#include "assetsystem/asset.hpp"
#include "assetsystem/iassetloader.hpp"
#include "assetsystem/texturedata.hpp"

namespace assets::loaders
{
    //-----------------------------------------------------------------------------

    using TexDataLoad = std::expected<TextureData*, std::string>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class TextureLoader : public IAssetLoader<TextureData>
    {
    public:
        ~TextureLoader() override = default;

        TexDataLoad Load( fs::IFileSystem* fileSystem, const Asset* asset ) override
        {
            if ( !asset )
                return std::unexpected( "Asset is invalid" );

            if ( asset->Id == ERROR_TEXTURE_ID )
                return LoadErrorTexture();

            if ( !fileSystem )
                return std::unexpected( "Filesystem is invalid" );

            auto buffer = fileSystem->Read( asset->Path );
            if ( buffer.empty() )
                return std::unexpected( "Failed to read file buffer from path: " + asset->Path.string() );

            int width, height, channels;
            unsigned char* image = stbi_load_from_memory(
                buffer.data(),
                static_cast<int>(buffer.size()),
                &width,
                &height,
                &channels,
                4 );

            if ( !image )
                return std::unexpected( "Couldn't load texture: " + asset->Path.string() );

            auto data = new TextureData();
            data->Image = image;
            data->Width = width;
            data->Height = height;
            data->Channels = channels;

            return data;
        }

        TextureData* LoadErrorTexture()
        {
            constexpr int width = 16;
            constexpr int height = 16;
            constexpr int channels = 4;
            constexpr int checkerSize = 4;

            auto data = new TextureData();
            data->Width = width;
            data->Height = height;
            data->Channels = channels;
            data->Image = static_cast<unsigned char*>(
                std::malloc( width * height * channels )
            );

            if ( !data->Image )
            {
                delete data;
                return nullptr;
            }

            for ( int y = 0; y < height; ++y )
            {
                for ( int x = 0; x < width; ++x )
                {
                    const bool magenta =
                            ( ( x / checkerSize ) + ( y / checkerSize ) ) % 2 == 0;

                    const int i = ( y * width + x ) * channels;

                    data->Image[ i + 0 ] = magenta ? 255 : 0;
                    data->Image[ i + 1 ] = 0;
                    data->Image[ i + 2 ] = magenta ? 255 : 0;
                    data->Image[ i + 3 ] = 255;
                }
            }

            return data;
        }
    };
}

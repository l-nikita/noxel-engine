#pragma once

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
            if ( !fileSystem ) return std::unexpected( "Filesystem is invalid" );
            if ( !asset ) return std::unexpected( "Asset is invalid" );

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
            data->Channels = 4;

            return data;
        }
    };
}

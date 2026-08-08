#pragma once

namespace assets
{
    //-----------------------------------------------------------------------------

    struct Asset;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    template<typename T>
    class IAssetLoader
    {
    public:
        virtual ~IAssetLoader() = default;

        virtual std::expected<T*, std::string> Load( fs::IFileSystem* fileSystem, const Asset* asset ) = 0;
    };
}

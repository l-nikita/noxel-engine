#pragma once

#include <filesystem>
#include <vector>
#include "common.hpp"

namespace fs
{
    //-----------------------------------------------------------------------------

    using Path = std::filesystem::path;
    using ByteBuffer = std::vector<std::uint8_t>;
    using Files = std::vector<Path>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class IFileSystem : public ISystem
    {
    public:
        virtual bool Exists( const Path& path ) const = 0;
        virtual bool IsDirectory( const Path& path ) const = 0;

        virtual Path Relative( const Path& path, const Path& basePath ) const = 0;
        virtual Path Relative( const Path& path ) const = 0;

        virtual Path Absolute( const Path& path ) const = 0;

        virtual ByteBuffer Read( const Path& path ) const = 0;
        virtual std::string ReadText( const Path& path ) const = 0;

        virtual bool Write( const Path& path, const ByteBuffer& data ) const = 0;
        virtual bool WriteText( const Path& path, std::string_view text ) const = 0;

        virtual void SearchFiles() = 0;
        virtual const Files& GetAllFiles() const = 0;

        virtual Path RootPath() const = 0;
    };
}

extern fs::IFileSystem* g_FileSystem;

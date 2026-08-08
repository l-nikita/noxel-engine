#pragma once

#include "ifilesystem.hpp"

namespace fs
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class FileSystem : public IFileSystem
    {
    public:
        SYSTEM_METADATA( FILESYSTEM, "FileSystem" );

    public:
        SysInit Initialize() override;
        void Shutdown() override;

        bool Exists( const Path& path ) const override;
        bool IsDirectory( const Path& path ) const override;

        Path Relative( const Path& path, const Path& basePath ) const override;
        Path Relative( const Path& path ) const override;

        Path Absolute( const Path& path ) const override;

        ByteBuffer Read( const Path& path ) const override;
        std::string ReadText( const Path& path ) const override;

        bool Write( const Path& path, const ByteBuffer& data ) const override;
        bool WriteText( const Path& path, std::string_view text ) const override;

        void SearchFiles() override;
        const Files& GetAllFiles() const override;

        Path RootPath() const override;

    private:
        Files m_files;
        Path m_rootPath;
    };
}

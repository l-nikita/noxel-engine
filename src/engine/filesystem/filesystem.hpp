#pragma once

#include "ifilesystem.hpp"

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

    bool Exists( const FPath& path ) const override;
    bool IsDirectory( const FPath& path ) const override;

    FPath Relative( const FPath& path, const FPath& basePath ) const override;
    FPath Relative( const FPath& path ) const override;

    FPath Absolute( const FPath& path ) const override;

    FByteBuffer Read( const FPath& path ) const override;
    std::string ReadText( const FPath& path ) const override;

    bool Write( const FPath& path, const FByteBuffer& data ) const override;
    bool WriteText( const FPath& path, std::string_view text ) const override;

    void SearchFiles() override;
    const FFiles& GetAllFiles() const override;

    FPath RootPath() const override;

private:
    FFiles m_files;
    FPath m_rootPath;
};

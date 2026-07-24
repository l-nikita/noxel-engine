#pragma once

#include <filesystem>
#include "common.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using FPath = std::filesystem::path;
using FByteBuffer = std::vector<std::uint8_t>;
using FFiles = std::vector<FPath>;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IFileSystem : public ISystem
{
public:
    virtual bool Exists( const FPath& path ) const = 0;
    virtual bool IsDirectory( const FPath& path ) const = 0;

    virtual FByteBuffer Read( const FPath& path ) const = 0;
    virtual std::string ReadText( const FPath& path ) const = 0;

    virtual bool Write( const FPath& path, const FByteBuffer& data ) const = 0;
    virtual bool WriteText( const FPath& path, std::string_view text ) const = 0;

    virtual void SearchFiles() = 0;
    virtual const FFiles& GetAllFiles() const = 0;

    virtual FPath GetRootPath() const = 0;
};

extern IFileSystem* g_FileSystem;

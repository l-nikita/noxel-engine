#pragma once

#include "filesystem/ifilesystem.hpp"

struct Asset;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T>
class IAssetLoader
{
public:
    virtual ~IAssetLoader() = default;

    virtual std::expected<T*, std::string> Load( IFileSystem* fileSystem, const Asset* asset ) = 0;
};

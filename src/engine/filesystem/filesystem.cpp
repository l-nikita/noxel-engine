#include "filesystem.hpp"

#include <fstream>
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SysInit FileSystem::Initialize()
{
    return {};
}

void FileSystem::Shutdown()
{
}

bool FileSystem::Exists( const FPath& path ) const
{
    return std::filesystem::exists( path );
}

FByteBuffer FileSystem::Read( const FPath& path ) const
{
    std::ifstream file( path, std::ios::binary | std::ios::ate );

    if ( !file )
        FATAL_ERROR( "Failed to open file: " + path.string(), CMD::FILESYSTEM );

    const auto end = file.tellg();
    if ( end < 0 )
        FATAL_ERROR( "Failed to read file size: " + path.string(), CMD::FILESYSTEM );

    FByteBuffer result( end );

    file.seekg( 0, std::ios::beg );

    if ( !result.empty() &&
         !file.read(
             reinterpret_cast<char*>(result.data()),
             static_cast<std::streamsize>(result.size()) ) )
    {
        FATAL_ERROR( "Failed to read file: " + path.string(), CMD::FILESYSTEM );
    }

    return result;
}

std::string FileSystem::ReadText( const FPath& path ) const
{
    std::ifstream file( path );

    if ( !file )
        FATAL_ERROR( "Failed to open file: " + path.string(), CMD::FILESYSTEM );

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

bool FileSystem::Write( const FPath& path, const FByteBuffer& data ) const
{
    std::filesystem::create_directories( path.parent_path() );
    std::ofstream file( path, std::ios::binary | std::ios::trunc );

    if ( !file )
        return false;

    file.write(
        reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()) );

    return file.good();
}

bool FileSystem::WriteText( const FPath& path, std::string_view text ) const
{
    std::filesystem::create_directories( path.parent_path() );
    std::ofstream file( path, std::ios::trunc );

    if ( !file )
        return false;

    file.write( text.data(), static_cast<std::streamsize>(text.size()) );

    return file.good();
}

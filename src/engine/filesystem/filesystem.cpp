#include "filesystem.hpp"

#include <fstream>
#include "dbg/debug.hpp"

namespace fs = std::filesystem;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SysInit FileSystem::Initialize()
{
    m_rootPath = fs::current_path();
    m_rootPath = m_rootPath / m_rootPath.parent_path();

    SearchFiles();

    CMD::RegisterCommand( "fs_print_files", "Prints files", [this]
    {
        for ( const auto& file: m_files )
            CMD_MSG( file.string(), CMD::FILESYSTEM );
    } );

    return {};
}

void FileSystem::Shutdown()
{
    m_files.clear();
}

bool FileSystem::Exists( const FPath& path ) const
{
    return fs::exists( path );
}

bool FileSystem::IsDirectory( const FPath& path ) const
{
    return fs::is_directory( path );
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
    fs::create_directories( path.parent_path() );
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
    fs::create_directories( path.parent_path() );
    std::ofstream file( path, std::ios::trunc );

    if ( !file )
        return false;

    file.write( text.data(), static_cast<std::streamsize>(text.size()) );

    return file.good();
}

void FileSystem::SearchFiles()
{
    m_files.clear();

    constexpr auto options = fs::directory_options::skip_permission_denied;
    for ( const auto& entry: fs::recursive_directory_iterator( GetRootPath(), options ) )
    {
        if ( !entry.is_regular_file() )
            continue;

        m_files.push_back( entry.path() );
    }
}

const FFiles& FileSystem::GetAllFiles() const
{
    return m_files;
}

FPath FileSystem::GetRootPath() const
{
    return m_rootPath;
}

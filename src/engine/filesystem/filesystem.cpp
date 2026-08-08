#include "filesystem.hpp"

#include <fstream>
#include "dbg/debug.hpp"

namespace fs
{
    //-----------------------------------------------------------------------------

    namespace stdfs = std::filesystem;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SysInit FileSystem::Initialize()
    {
        m_rootPath = stdfs::current_path();
        m_rootPath = m_rootPath / m_rootPath.parent_path();

        SearchFiles();

        cmd::RegisterCommand( "fs_print_files", "Prints files", [this]
        {
            for ( const auto& file: m_files )
                CMD_MSG( file.string(), cmd::FILESYSTEM );
        } );

        return {};
    }

    void FileSystem::Shutdown()
    {
        m_files.clear();
    }

    bool FileSystem::Exists( const Path& path ) const
    {
        return stdfs::exists( path.is_relative() ? ( m_rootPath / path ) : path );
    }

    bool FileSystem::IsDirectory( const Path& path ) const
    {
        return stdfs::is_directory( path );
    }

    Path FileSystem::Relative( const Path& path, const Path& basePath ) const
    {
        return stdfs::relative( path, basePath );
    }

    Path FileSystem::Relative( const Path& path ) const
    {
        return stdfs::relative( path, m_rootPath );
    }

    Path FileSystem::Absolute( const Path& path ) const
    {
        return m_rootPath / path;
    }

    ByteBuffer FileSystem::Read( const Path& path ) const
    {
        const Path fPath = path.is_relative() ? ( m_rootPath / path ) : path;

        std::ifstream file( fPath, std::ios::binary | std::ios::ate );

        if ( !file )
            FATAL_ERROR( "Failed to open file: " + fPath.string(), cmd::FILESYSTEM );

        const auto end = file.tellg();
        if ( end < 0 )
            FATAL_ERROR( "Failed to read file size: " + fPath.string(), cmd::FILESYSTEM );

        ByteBuffer result( end );

        file.seekg( 0, std::ios::beg );

        if ( !result.empty() &&
             !file.read(
                 reinterpret_cast<char*>(result.data()),
                 static_cast<std::streamsize>(result.size()) ) )
        {
            FATAL_ERROR( "Failed to read file: " + fPath.string(), cmd::FILESYSTEM );
        }

        return result;
    }

    std::string FileSystem::ReadText( const Path& path ) const
    {
        const Path fPath = path.is_relative() ? ( m_rootPath / path ) : path;

        std::ifstream file( fPath );

        if ( !file )
            FATAL_ERROR( "Failed to open file: " + fPath.string(), cmd::FILESYSTEM );

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }

    bool FileSystem::Write( const Path& path, const ByteBuffer& data ) const
    {
        const Path fPath = path.is_relative() ? ( m_rootPath / path ) : path;

        stdfs::create_directories( fPath.parent_path() );
        std::ofstream file( fPath, std::ios::binary | std::ios::trunc );

        if ( !file )
            return false;

        file.write(
            reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size()) );

        return file.good();
    }

    bool FileSystem::WriteText( const Path& path, std::string_view text ) const
    {
        const Path fPath = path.is_relative() ? ( m_rootPath / path ) : path;

        stdfs::create_directories( fPath.parent_path() );
        std::ofstream file( fPath, std::ios::trunc );

        if ( !file )
            return false;

        file.write( text.data(), static_cast<std::streamsize>(text.size()) );

        return file.good();
    }

    void FileSystem::SearchFiles()
    {
        m_files.clear();

        constexpr auto options = stdfs::directory_options::skip_permission_denied;
        for ( const auto& entry: stdfs::recursive_directory_iterator( RootPath(), options ) )
        {
            if ( !entry.is_regular_file() )
                continue;

            m_files.push_back( entry.path() );
        }
    }

    const Files& FileSystem::GetAllFiles() const
    {
        return m_files;
    }

    Path FileSystem::RootPath() const
    {
        return m_rootPath;
    }
}

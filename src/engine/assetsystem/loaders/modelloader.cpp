#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE

#include <nlohmann/json.hpp>
#include <stb_image.h>
#include <stb_image_write.h>
#include <tiny_gltf.h>

#include "assetsystem/loaders/modelloader.hpp"

//-----------------------------------------------------------------------------
inline void from_json( const nlohmann::json& json, ModelData& data )
{
    data.MeshPath = json.value( "meshpath", "" );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ModelDataLoad ModelLoader::Load( IFileSystem* fileSystem, const Asset* asset )
{
    if ( !fileSystem ) return std::unexpected( "Filesystem is invalid" );
    if ( !asset ) return std::unexpected( "Asset is invalid" );

    auto jsonStr = fileSystem->ReadText( asset->Path );
    if ( !jsonStr.data() || jsonStr.empty() )
        return std::unexpected( "Model meta is invalid: " + asset->Path.string() );

    try
    {
        nlohmann::json json = nlohmann::json::parse( jsonStr );
        auto data = new ModelData( json.get<ModelData>() );

        auto meshPath = asset->Path.parent_path() / data->MeshPath;
        if ( !g_FileSystem->Exists( meshPath ) )
        {
            meshPath = data->MeshPath;
            if ( !g_FileSystem->Exists( meshPath ) )
                return std::unexpected( "Couldn't find mesh file: " + meshPath.string() );
        }

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        std::string err;
        std::string warn;

        auto buffer = fileSystem->Read( meshPath );
        if ( buffer.empty() )
            return std::unexpected( "Failed to read file buffer from path: " + asset->Path.string() );

        bool loaded = loader.LoadBinaryFromMemory( &model, &err, &warn, buffer.data(), buffer.size() );
        if ( !loaded )
            return std::unexpected( "Failed to parse glTF file: " + asset->Path.string() );

        if ( !warn.empty() )
            CMD_WARNING( "glTF Warning: " + asset->Path.string() + " - " + warn, CMD::ASSETSYSTEM );

        if ( !err.empty() )
            CMD_ERROR( "glTF Error: " + asset->Path.string() + " - " + err, CMD::ASSETSYSTEM );

        auto mesh = model.meshes.front();
        CMD_DMSG( std::to_string( mesh.primitives.size() ) );

        return data;
    } catch ( const nlohmann::json::exception& e )
    {
        return std::unexpected(
            "JSON parse error: " + std::string( e.what() ) + "\nFile: " + asset->Path.string() );
    }
}

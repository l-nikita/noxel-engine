#pragma once

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include "assettype.hpp"
#include "filesystem/ifilesystem.hpp"

namespace assets
{
    using Id = boost::uuids::uuid;

    constexpr auto ERROR_TEXTURE_ID = boost::uuids::string_generator()(
        "00000000-0000-0000-0000-000000000001"
    );

    struct Asset
    {
        assets::Id Id;
        std::string Name;
        fs::Path Path;
        assets::Type Type;
    };

    struct IAssetData
    {
        virtual ~IAssetData() = default;
    };

    constexpr auto META_EXTENSION = ".meta";

    static const std::string SUPPORTED_IMAGE_FORMATS[ ] = {
        ".jpg", ".png"
    };

    static const std::string SUPPORTED_MESH_FORMATS[ ] = {
        ".glb", ".gltf"
    };

    constexpr auto MATERIAL_EXTENSION = ".nmt";
    constexpr auto SHADER_EXTENSION = ".nshader";
    constexpr auto MODEL_EXTENSION = ".nmdl";
}

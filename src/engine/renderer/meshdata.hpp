#pragma once

#include <vector>
#include "renderer/vertex.hpp"

namespace render
{
    struct MeshData
    {
        std::vector<Vertex> Vertices;
        std::vector<std::uint32_t> Indices;
    };
}

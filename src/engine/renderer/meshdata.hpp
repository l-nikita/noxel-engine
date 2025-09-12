#pragma once

#include <vector>
#include "vertex.hpp"

struct MeshData 
{
    std::vector<Vertex> Vertices;
    std::vector<std::uint32_t> Indices;
};
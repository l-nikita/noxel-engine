#pragma once

#include <vector>
#include "vertex.hpp"

struct MeshData 
{
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
};
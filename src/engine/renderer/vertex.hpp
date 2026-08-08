#pragma once

#include <glm/glm.hpp>

namespace render
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        glm::vec2 UV;
        glm::vec4 Color;
    };
}

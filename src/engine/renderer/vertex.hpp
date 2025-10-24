#pragma once

#include <glm/glm.hpp>

struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
    glm::vec4 Color;
};

struct CBLightData
{
    glm::vec3 lightPos;
    float pad0;
    glm::vec3 lightColor;
    float pad1;
    glm::vec3 cameraPos;
    float pad2;
};
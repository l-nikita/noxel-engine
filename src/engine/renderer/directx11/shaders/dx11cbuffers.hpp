#pragma once

#include "glm/glm.hpp"

struct CBTransform
{
    glm::mat4 Model;
    glm::mat4 View;
    glm::mat4 Projection;
};

struct CBColor
{
    glm::vec4 Color;
};

enum LightType
{
    LIGHT_POINT,
    LIGHT_SPOT,
    LIGHT_DIRECTIONAL,
};

struct CBLight
{
    int Type;
    int pad0, pad1, pad2;
    glm::vec3 Pos;
    float Radius;
    glm::vec3 Color;
    float Cutoff;
    glm::vec3 Dir;
    int pad3;
};

struct CBLightsData
{
    CBLight Lights[4];
    glm::vec3 ViewPos;
    float pad0;
};
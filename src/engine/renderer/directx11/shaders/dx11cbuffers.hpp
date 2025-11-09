#pragma once

#include "glm/glm.hpp"
#include "renderer/lighting.hpp"

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

struct alignas(16) CBLight
{
    int Type;
    float _pad0, _pad1, _pad2;
    glm::vec3 Pos;
    float _pad3;
    glm::vec3 Dir;
    float _pad4;
    glm::vec3 Color;
    float _pad5;
    float Radius;
    float Cutoff;
    int ShadowMapIndex;
    float _pad6;
    glm::mat4 View;
    glm::mat4 Projection;
};

struct CBLightsData
{
    CBLight Lights[MAX_LIGHTS];
    glm::vec3 ViewPos;
    float pad0;
};
#pragma once

#include "glm/glm.hpp"

struct CBMatrices
{
    glm::mat4 Model;
    glm::mat4 View;
    glm::mat4 Projection;
};

struct CBColor
{
    glm::vec4 Color;
};
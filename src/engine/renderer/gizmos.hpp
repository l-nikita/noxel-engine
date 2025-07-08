#pragma once

#include <glm/glm.hpp>

namespace Gizmos
{
    void Init();
    void Line(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f));
    void Shutdown();
}
#pragma once

#include <glm/glm.hpp>

namespace Gizmos
{
    void Init();

    void Line(
        const glm::vec3& start,
        const glm::vec3& end,
        const glm::vec4& color = glm::vec4(1.0f)
    );

    void Box(
        const glm::vec3& position,
        const glm::quat& rotation,
        const glm::vec3& min,
        const glm::vec3& max,
        const glm::vec4& color = glm::vec4(1.0f)
    );

    void Triangle(
        const glm::vec3& position,
        const glm::vec3& v1,
        const glm::vec3& v2,
        const glm::vec3& v3,
        const glm::vec4& color = glm::vec4(1.0f)
    );

    void Shutdown();
}
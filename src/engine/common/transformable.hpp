#pragma once

#include "common/transform.hpp"

class Transformable
{
public:
	Transform m_Transform;
    glm::mat4 GetTransformMatrix() const { return m_Transform.GetTransformMatrix(); }

    const Transform& GetTransform() const { return m_Transform; }
    void SetTransform(const Transform& transform) { m_Transform = transform; }
};
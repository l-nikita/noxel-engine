#pragma once

#include "common/transform.hpp"

class Transformable
{
public:
	Transform m_Transform;
    glm::mat4 GetTransformMatrix() const { return m_Transform.GetTransformMatrix(); }
    const Transform& GetTransform() const { return m_Transform; }

    void SetTransform(const Transform& transform) { m_Transform = transform; }

    glm::vec3 GetPosition() const { return m_Transform.Position; }
    void SetPosition(const glm::vec3& pos) { m_Transform.Position = pos; }

    glm::quat GetRotation() const { return m_Transform.Rotation; }
    void SetRotation(const glm::quat& rot) { m_Transform.Rotation = rot; }
};
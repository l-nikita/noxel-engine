#pragma once

#include "transform.hpp"

class Transformable
{
public:
	Transform m_Transform;
    glm::mat4 GetTransformMatrix() const { return m_Transform.GetTransformMatrix(); }
};
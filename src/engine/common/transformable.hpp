#pragma once

#include "common/transform.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T>
class Transformable
{
public:
    Transform m_Transform;

    glm::mat4 GetTransformMatrix() const
    {
        return m_Transform.GetTransformMatrix();
    }

    const Transform& GetTransform() const
    {
        return m_Transform;
    }

    T& SetTransform( const Transform& transform )
    {
        m_Transform = transform;
        return static_cast<T &>(*this);
    }

    glm::vec3 GetPosition() const
    {
        return m_Transform.Position;
    }

    T& SetPosition( const glm::vec3& pos )
    {
        m_Transform.Position = pos;
        return static_cast<T &>(*this);
    }

    glm::quat GetRotation() const
    {
        return m_Transform.Rotation;
    }

    T& SetRotation( const glm::quat& rot )
    {
        m_Transform.Rotation = rot;
        return static_cast<T &>(*this);
    }

    T& LookAt( const glm::vec3& target, const glm::vec3& up = TRANSFORM_UP )
    {
        m_Transform.LookAt( target, up );
        return static_cast<T &>(*this);
    }

    glm::vec3 Forward() const
    {
        return m_Transform.Forward();
    }

    glm::vec3 Backward() const
    {
        return m_Transform.Backward();
    }

    glm::vec3 Right() const
    {
        return m_Transform.Right();
    }

    glm::vec3 Left() const
    {
        return m_Transform.Left();
    }

    glm::vec3 Up() const
    {
        return m_Transform.Up();
    }

    glm::vec3 Down() const
    {
        return m_Transform.Down();
    }
};

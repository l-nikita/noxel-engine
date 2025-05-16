#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "angles.hpp"

struct Transform 
{
    glm::vec3 Position = glm::vec3(0.0f);
    Angles Rotation = Angles::Zero();
    glm::vec3 Scale = glm::vec3(1.0f);

    glm::vec3 Backward() const { return -Rotation.GetForward(); }
    glm::vec3 Forward() const { return Rotation.GetForward(); }
    glm::vec3 Right() const { return Rotation.GetRight(); }
    glm::vec3 Left() const { return -Rotation.GetRight(); }
    glm::vec3 Up() const { return Rotation.GetUp(); }
    glm::vec3 Down() const { return -Rotation.GetUp(); }

    Transform() = default;

    Transform(const glm::vec3& pos)
        : Position(pos), Rotation(Angles::Zero()), Scale(1.0f) {}

    Transform(const glm::vec3& position, const Angles& rotation, float scale = 1.0f)
        : Position(position), Rotation(rotation), Scale(scale) {}

    Transform(const glm::vec3& position, const Angles& rotation, const glm::vec3& scale)
        : Position(position), Rotation(rotation), Scale(scale) {}

    glm::mat4 GetTransformMatrix() const 
    {
        glm::mat4 S = glm::scale(glm::mat4(1.0f), Scale);
        glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.Pitch), glm::vec3(1, 0, 0));
        glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.Yaw), glm::vec3(0, 1, 0));
        glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.Roll), glm::vec3(0, 0, 1));
        glm::mat4 T = glm::translate(glm::mat4(1.0f), Position);

        return T * Ry * Rx * Rz * S;
    }

    bool operator==(const Transform& other) const 
    {
        return Position == other.Position &&
            Scale == other.Scale &&
            Rotation == other.Rotation;
    }

    bool operator!=(const Transform& other) const 
    {
        return !(*this == other);
    }

    Transform WithPosition(const glm::vec3& pos) const 
    {
        Transform result = *this;
        result.Position = pos;
        return result;
    }

    Transform WithPosition(const glm::vec3& pos, const Angles& rot) const 
    {
        Transform result = *this;
        result.Position = pos;
        result.Rotation = rot;
        return result;
    }

    Transform WithRotation(const Angles& rot) const 
    {
        Transform result = *this;
        result.Rotation = rot;
        return result;
    }

    Transform WithScale(float scale) const 
    {
        Transform result = *this;
        result.Scale = glm::vec3(scale);
        return result;
    }

    Transform WithScale(const glm::vec3& scale) const 
    {
        Transform result = *this;
        result.Scale = scale;
        return result;
    }
};

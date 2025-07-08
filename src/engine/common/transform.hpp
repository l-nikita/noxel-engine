#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <sstream>

struct Transform
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
    glm::vec3 Scale = glm::vec3(1.0f);

    Transform() = default;

    Transform(const glm::vec3& pos)
        : Position(pos), Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), Scale(1.0f) 
    {}

    Transform(const glm::vec3& position, const glm::quat& rotation, float scale = 1.0f)
        : Position(position), Rotation(rotation), Scale(scale) 
    {}

    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
        : Position(position), Rotation(rotation), Scale(scale) 
    {}

    glm::mat4 GetTransformMatrix() const
    {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), Position);
        glm::mat4 R = glm::toMat4(Rotation);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), Scale);
        return T * R * S;
    }

    glm::vec3 Forward()  const { return Rotation * glm::vec3(0.0f, 0.0f, 1.0f); }  // +Z
    glm::vec3 Backward() const { return Rotation * glm::vec3(0.0f, 0.0f, -1.0f); } // -Z
    glm::vec3 Right()    const { return Rotation * glm::vec3(1.0f, 0.0f, 0.0f); }  // +X
    glm::vec3 Left()     const { return Rotation * glm::vec3(-1.0f, 0.0f, 0.0f); } // -X
    glm::vec3 Up()       const { return Rotation * glm::vec3(0.0f, 1.0f, 0.0f); }  // +Y
    glm::vec3 Down()     const { return Rotation * glm::vec3(0.0f, -1.0f, 0.0f); } // -Y

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
        return Transform(pos, Rotation, Scale);
    }

    Transform WithRotation(const glm::quat& rot) const
    {
        return Transform(Position, rot, Scale);
    }

    Transform WithScale(float scale) const
    {
        return Transform(Position, Rotation, glm::vec3(scale));
    }

    Transform WithScale(const glm::vec3& scale) const
    {
        return Transform(Position, Rotation, scale);
    }

    std::string ToString() const
    {
        std::ostringstream ss;
        ss << "Position: (" << Position.x << ", " << Position.y << ", " << Position.z << "), ";
        ss << "Rotation: (" << Rotation.w << ", " << Rotation.x << ", " << Rotation.y << ", " << Rotation.z << "), ";
        ss << "Scale: (" << Scale.x << ", " << Scale.y << ", " << Scale.z << ")";
        return ss.str();
    }
};
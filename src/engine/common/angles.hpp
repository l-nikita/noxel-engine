#pragma once

#include <glm/glm.hpp>

static float NormalizeAngle(float& angle)
{
    angle = fmodf(angle, 360.0f);
    if (angle < -180.0f)
        angle += 360.0f;

    if (angle >= 180.0f)
        angle -= 360.0f;

    return angle;
}

struct Angles 
{
    float Pitch = 0.0f; // X
    float Yaw = 0.0f; // Y
    float Roll = 0.0f; // Z

    static inline Angles Zero() { return Angles(); }

    glm::vec3 GetForward() const 
    {
        float pitchRad = glm::radians(Pitch);
        float yawRad = glm::radians(Yaw);

        return glm::normalize(glm::vec3(
            cos(pitchRad) * sin(yawRad),
            sin(pitchRad),
            cos(pitchRad) * cos(yawRad)
        ));
    }

    glm::vec3 GetRight() const 
    {
        return glm::normalize(glm::cross(GetForward(), glm::vec3(0, 1, 0)));
    }

    glm::vec3 GetUp() const 
    {
        return glm::normalize(glm::cross(GetRight(), GetForward()));
    }

    void Normalize()
    {
        Pitch = NormalizeAngle(Pitch);
        Yaw = NormalizeAngle(Yaw);
        Roll = NormalizeAngle(Roll);
    }

    bool operator==(const Angles& other) const 
    {
        return Pitch == other.Pitch 
            && Yaw == other.Yaw 
            && Roll == other.Roll;
    }
};
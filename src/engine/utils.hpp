#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Jolt.h>

namespace Utils
{
    inline JPH::RVec3 JPH_Vec3_GLM( const glm::vec3& vec3 )
    {
        return JPH::RVec3(vec3.x, vec3.y, vec3.z);
    }

    inline glm::vec3 GLM_Vec3_JPH( const JPH::RVec3& vec3 )
    {
        return glm::vec3(vec3.GetX(), vec3.GetY(), vec3.GetZ());
    }

    inline JPH::Quat JPH_Quat_GLM( const glm::quat& quat )
    {
        return JPH::Quat(quat.x, quat.y, quat.z, quat.w);
    }

    inline glm::quat GLM_Quat_JPH( const JPH::Quat& quat )
    {
        return glm::quat(quat.GetW(), quat.GetX(), quat.GetY(), quat.GetZ());
    }

    inline glm::vec4 GLM_Color_JPH( const JPH::Color& color )
    {
        return glm::vec4(color.r, color.g, color.b, color.a);
    }
}

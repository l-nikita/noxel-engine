#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>

namespace utils
{
    inline JPH::RVec3 JPH_Vec3_GLM( const glm::vec3& vec3 )
    {
        return {vec3.x, vec3.y, vec3.z};
    }

    inline glm::vec3 GLM_Vec3_JPH( const JPH::RVec3& vec3 )
    {
        return {vec3.GetX(), vec3.GetY(), vec3.GetZ()};
    }

    inline JPH::Quat JPH_Quat_GLM( const glm::quat& quat )
    {
        return {quat.x, quat.y, quat.z, quat.w};
    }

    inline glm::quat GLM_Quat_JPH( const JPH::Quat& quat )
    {
        return {quat.GetW(), quat.GetX(), quat.GetY(), quat.GetZ()};
    }

    inline glm::vec4 GLM_Color_JPH( const JPH::Color& color )
    {
        return {color.r, color.g, color.b, color.a};
    }

    inline std::string String_Vec3_JPH( const JPH::RVec3& vec3 )
    {
        return std::to_string( vec3.GetX() ) + " " + std::to_string( vec3.GetY() ) + " " +
               std::to_string( vec3.GetZ() );
    }

    inline std::string String_Vec3_GLM( const glm::vec3& vec3 )
    {
        return std::to_string( vec3.x ) + " " + std::to_string( vec3.y ) + " " +
               std::to_string( vec3.z );
    }

#define DEFINE_ENUM_FLAG_OPERATORS(Enum) \
    constexpr Enum operator|(Enum lhs, Enum rhs) noexcept { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) | static_cast<std::underlying_type_t<Enum>>(rhs)); \
    } \
    constexpr Enum operator&(Enum lhs, Enum rhs) noexcept { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) & static_cast<std::underlying_type_t<Enum>>(rhs)); \
    } \
    constexpr Enum operator~(Enum val) noexcept { \
        return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(val)); \
    } \
    inline Enum& operator|=(Enum& lhs, Enum rhs) noexcept { \
        return lhs = lhs | rhs; \
    } \
    inline Enum& operator&=(Enum& lhs, Enum rhs) noexcept { \
        return lhs = lhs & rhs; \
    } \
    constexpr bool Any(Enum val) noexcept { \
        return static_cast<std::underlying_type_t<Enum>>(val) != 0; \
    } \
    constexpr bool HasFlag(Enum combined, Enum flag) noexcept { \
        return Any(combined & flag); \
    }
}

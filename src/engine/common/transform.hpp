#pragma once

#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

//-----------------------------------------------------------------------------

constexpr glm::vec3 TRANSFORM_FORWARD = glm::vec3( 0.0f, 0.0f, 1.0f );
constexpr glm::vec3 TRANSFORM_RIGHT = glm::vec3( 1.0f, 0.0f, 0.0f );
constexpr glm::vec3 TRANSFORM_UP = glm::vec3( 0.0f, 1.0f, 0.0f );

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct Transform
{
    glm::vec3 Position = glm::vec3( 0.0f );
    glm::quat Rotation = glm::quat( 1.0f, 0.0f, 0.0f, 0.0f ); // Identity quaternion
    glm::vec3 Scale = glm::vec3( 1.0f );

    Transform() = default;

    explicit Transform( const glm::vec3& pos )
        : Position( pos ), Scale( 1.0f )
    {
    }

    Transform( const glm::vec3& position, const glm::quat& rotation, float scale = 1.0f )
        : Position( position ), Rotation( rotation ), Scale( scale )
    {
    }

    Transform( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale )
        : Position( position ), Rotation( rotation ), Scale( scale )
    {
    }

    glm::mat4 GetTransformMatrix() const
    {
        glm::mat4 T = glm::translate( glm::mat4( 1.0f ), Position );
        glm::mat4 R = glm::toMat4( Rotation );
        glm::mat4 S = glm::scale( glm::mat4( 1.0f ), Scale );
        return T * R * S;
    }

    // +Z
    glm::vec3 Forward() const
    {
        return glm::normalize( Rotation * TRANSFORM_FORWARD );
    }

    // -Z
    glm::vec3 Backward() const
    {
        return glm::normalize( Rotation * -TRANSFORM_FORWARD );
    }

    // +X
    glm::vec3 Right() const
    {
        return glm::normalize( Rotation * TRANSFORM_RIGHT );
    }

    // -X
    glm::vec3 Left() const
    {
        return glm::normalize( Rotation * -TRANSFORM_RIGHT );
    }

    // +Y
    glm::vec3 Up() const
    {
        return glm::normalize( Rotation * TRANSFORM_UP );
    }

    // -Y
    glm::vec3 Down() const
    {
        return glm::normalize( Rotation * -TRANSFORM_UP );
    }

    glm::vec3 EulerAngles() const
    {
        return glm::eulerAngles( Rotation );
    }

    void LookAt( const glm::vec3& target, const glm::vec3& up = TRANSFORM_UP )
    {
        glm::vec3 dir = glm::normalize( target - Position );
        Rotation = glm::normalize( glm::quatLookAtLH( dir, up ) );
    }

    Transform WithPosition( const glm::vec3& pos ) const
    {
        return {pos, Rotation, Scale};
    }

    Transform WithRotation( const glm::quat& rot ) const
    {
        return {Position, rot, Scale};
    }

    Transform WithScale( float scale ) const
    {
        return {Position, Rotation, glm::vec3( scale )};
    }

    Transform WithScale( const glm::vec3& scale ) const
    {
        return {Position, Rotation, scale};
    }

    Transform WithEulerAngles( const glm::vec3& radians ) const
    {
        return {Position, glm::quat( radians ), Scale};
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

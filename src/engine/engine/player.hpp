#pragma once

#include <algorithm>
#include <limits>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

#include "engine.hpp"
#include "window.hpp"
#include "input/iinput.hpp"
#include "physics/physics.hpp"
#include "physics/physics_renderer.hpp"
#include "renderer/camera.hpp"

constexpr auto SENSITIVITY = 0.15f;
constexpr auto CHARACTER_RADIUS = 0.3f;
constexpr auto CHARACTER_HEIGHT = 1.7f;
constexpr auto CHARACTER_EYE_HEIGHT = 1.65f;
constexpr auto CHARACTER_MAX_SPEED = 7.0f;
constexpr auto CHARACTER_GROUND_ACCELERATION = 45.0f;
constexpr auto CHARACTER_GROUND_DECELERATION = 35.0f;
constexpr auto CHARACTER_AIR_ACCELERATION = 8.0f;
constexpr auto CHARACTER_JUMP_SPEED = 8.0f;
constexpr auto CHARACTER_GRAVITY_SCALE = 2.0f;
constexpr auto CHARACTER_JUMP_BUFFER_TIME = 0.12f;
constexpr auto CHARACTER_COYOTE_TIME = 0.1f;

class Player
{
public:
    explicit Player( render::Camera* camera )
        : m_Camera( camera )
    {
        static_assert( CHARACTER_HEIGHT > 2.0f * CHARACTER_RADIUS );

        const JPH::Ref settings = new JPH::CharacterVirtualSettings();
        settings->mShape = JPH::RotatedTranslatedShapeSettings(
            JPH::Vec3( 0.0f, 0.5f * CHARACTER_HEIGHT, 0.0f ),
            JPH::Quat::sIdentity(),
            new JPH::CapsuleShape( 0.5f * ( CHARACTER_HEIGHT - 2.0f * CHARACTER_RADIUS ),
                                   CHARACTER_RADIUS )
        ).Create().Get();
        settings->mSupportingVolume = JPH::Plane( JPH::Vec3::sAxisY(), -CHARACTER_RADIUS );

        m_character = new JPH::CharacterVirtual( settings, JPH::RVec3( 0.0f, 1.0f, 0.0f ), JPH::Quat::sIdentity(),
                                                 phys::GetPhysics()->GetSystem() );
        m_character->SetUp( JPH::Vec3( 0, 1, 0 ) );

        m_previousCharacterPosition = Utils::GLM_Vec3_JPH( m_character->GetPosition() );
        m_currentCharacterPosition = m_previousCharacterPosition;
    }

    void UpdateCamera()
    {
        JPH::RayCastResult hit;
        const JPH::RRayCast ray(
            Utils::JPH_Vec3_GLM( m_Camera->GetPosition() ),
            Utils::JPH_Vec3_GLM( m_Camera->Backward() ) * 15
        );

        Raycast( phys::GetPhysics()->GetSystem(), ray, hit );

        const auto feetPos = glm::mix( m_previousCharacterPosition, m_currentCharacterPosition,
                                       g_Engine->FixedAlpha() );
        auto eyePos = feetPos + glm::vec3( 0, CHARACTER_EYE_HEIGHT, 0 );

        if ( m_thirdPerson )
        {
            constexpr auto targetDistance = 3.0f;
            const glm::vec3 rayDir = m_Camera->Backward();

            JPH::RayCastResult hit;
            const JPH::RRayCast ray(
                Utils::JPH_Vec3_GLM( eyePos ),
                Utils::JPH_Vec3_GLM( rayDir * targetDistance * ( targetDistance / 2 ) )
            );

            if ( Raycast( phys::GetPhysics()->GetSystem(), ray, hit ) )
                eyePos += rayDir * ( targetDistance * ( hit.mFraction ) );
            else
                eyePos += rayDir * targetDistance;
        }

        m_Camera->SetPosition( eyePos );
        UpdateAimRay();
    }

    glm::vec3 GetMovementDir() const
    {
        auto dir = glm::vec3( 0 );

        const auto forward = glm::vec3( m_Camera->Forward().x, 0, m_Camera->Forward().z );
        const auto right = glm::vec3( m_Camera->Right().x, 0, m_Camera->Right().z );

        if ( g_Input->IsKeyDown( SDL_SCANCODE_W ) ) dir += forward;
        if ( g_Input->IsKeyDown( SDL_SCANCODE_S ) ) dir -= forward;
        if ( g_Input->IsKeyDown( SDL_SCANCODE_A ) ) dir -= right;
        if ( g_Input->IsKeyDown( SDL_SCANCODE_D ) ) dir += right;

        if ( glm::length( dir ) > 0.0f )
            dir = glm::normalize( dir );

        return dir;
    }

    bool IsOnGround() const
    {
        return m_character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround;
    }

    static glm::vec3 MoveTowards( const glm::vec3& current, const glm::vec3& target, const float maxDelta )
    {
        const auto delta = target - current;
        const auto distance = glm::length( delta );

        if ( distance <= maxDelta || distance <= std::numeric_limits<float>::epsilon() )
            return target;

        return current + delta * ( maxDelta / distance );
    }

    glm::vec3 GetWishVelocity() const
    {
        return m_moveInput * CHARACTER_MAX_SPEED;
    }

    glm::vec3 StartPos = {};
    glm::vec3 HitPos = {};

    void UpdateMovement( float dt )
    {
        m_character->UpdateGroundVelocity();

        const auto up = Utils::GLM_Vec3_JPH( m_character->GetUp() );
        const auto currentVelocity = Utils::GLM_Vec3_JPH( m_character->GetLinearVelocity() );
        const auto currentVerticalVelocity = glm::dot( currentVelocity, up ) * up;
        const auto currentHorizontalVelocity = currentVelocity - currentVerticalVelocity;
        const auto groundVelocity = Utils::GLM_Vec3_JPH( m_character->GetGroundVelocity() );
        const auto groundVerticalVelocity = glm::dot( groundVelocity, up ) * up;
        const auto groundHorizontalVelocity = groundVelocity - groundVerticalVelocity;
        const auto movingTowardsGround = glm::dot( currentVerticalVelocity - groundVerticalVelocity, up ) < 0.1f;
        const auto grounded = IsOnGround() && movingTowardsGround;

        if ( grounded )
            m_coyoteTimeRemaining = CHARACTER_COYOTE_TIME;
        else
            m_coyoteTimeRemaining = std::max( 0.0f, m_coyoteTimeRemaining - dt );

        m_jumpBufferRemaining = std::max( 0.0f, m_jumpBufferRemaining - dt );

        const auto wishVelocity = GetWishVelocity();
        const auto jumpRequested = m_jumpBufferRemaining > 0.0f && m_coyoteTimeRemaining > 0.0f;
        glm::vec3 velocity;

        if ( grounded )
        {
            const auto relativeHorizontalVelocity = currentHorizontalVelocity - groundHorizontalVelocity;
            const auto acceleration = glm::dot( m_moveInput, m_moveInput ) > 0.0f
                                          ? CHARACTER_GROUND_ACCELERATION
                                          : CHARACTER_GROUND_DECELERATION;
            velocity = groundVelocity + MoveTowards( relativeHorizontalVelocity, wishVelocity, acceleration * dt );
        } else
        {
            auto horizontalVelocity = currentHorizontalVelocity;
            if ( glm::dot( m_moveInput, m_moveInput ) > 0.0f )
                horizontalVelocity = MoveTowards( horizontalVelocity, wishVelocity,
                                                  CHARACTER_AIR_ACCELERATION * dt );

            velocity = currentVerticalVelocity + horizontalVelocity;
        }

        if ( jumpRequested )
        {
            velocity += CHARACTER_JUMP_SPEED * up;
            m_jumpBufferRemaining = 0.0f;
            m_coyoteTimeRemaining = 0.0f;
        }

        velocity += g_Physics->GetGravity() * CHARACTER_GRAVITY_SCALE * dt;
        m_character->SetLinearVelocity( Utils::JPH_Vec3_GLM( velocity ) );
    }

    bool Raycast(
        const JPH::PhysicsSystem* physicsSystem,
        const JPH::RRayCast& ray,
        JPH::RayCastResult& hit
    )
    {
        return physicsSystem->GetNarrowPhaseQuery()
                .CastRay( ray, hit );
    }

    void UpdateAimRay()
    {
        StartPos = m_Camera->GetPosition();

        JPH::RayCastResult hit;
        const JPH::RRayCast ray(
            Utils::JPH_Vec3_GLM( StartPos ),
            Utils::JPH_Vec3_GLM( m_Camera->Forward() ) * 5
        );

        if ( Raycast( phys::GetPhysics()->GetSystem(), ray, hit ) )
            HitPos = Utils::GLM_Vec3_JPH( ray.GetPointOnRay( hit.mFraction ) );
        else
            HitPos = StartPos + m_Camera->Forward() * 5.0f;
    }

    void Update()
    {
        UpdateCamera();
    }

    static bool CanControl()
    {
        return g_Input->GetRelativeMouseMode() && g_Engine->GetPrimaryWindow()->IsInputFocused();
    }

    void HandleInput()
    {
        const auto delta = g_Input->GetMouseDelta();

        if ( !CanControl() )
        {
            m_moveInput = {};
            m_jumpBufferRemaining = 0.0f;
            return;
        }

        m_yaw += delta.x * SENSITIVITY;
        m_pitch += delta.y * SENSITIVITY;
        m_pitch = std::clamp( m_pitch, -85.0f, 85.0f );

        const auto qPitch = glm::angleAxis( glm::radians( m_pitch ), glm::vec3( 1, 0, 0 ) );
        const auto qYaw = glm::angleAxis( glm::radians( m_yaw ), glm::vec3( 0, 1, 0 ) );
        m_Camera->SetRotation( qYaw * qPitch );

        if ( g_Input->IsKeyPressed( SDL_SCANCODE_C ) )
            m_thirdPerson = !m_thirdPerson;

        m_moveInput = GetMovementDir();

        if ( g_Input->IsKeyPressed( SDL_SCANCODE_SPACE ) )
            m_jumpBufferRemaining = CHARACTER_JUMP_BUFFER_TIME;
    }

    void FixedUpdate( float dt )
    {
        m_previousCharacterPosition = m_currentCharacterPosition;
        UpdateMovement( dt );
        UpdatePhysics( dt );
        m_currentCharacterPosition = Utils::GLM_Vec3_JPH( m_character->GetPosition() );
    }

    void UpdatePhysics( float dt ) const
    {
        const JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;
        m_character->ExtendedUpdate( dt,
                                     CHARACTER_GRAVITY_SCALE * g_Physics->GetGravity().length() *
                                     -m_character->GetUp(),
                                     updateSettings,
                                     phys::GetPhysics()->GetSystem()->GetDefaultBroadPhaseLayerFilter(
                                         phys::Layers::MOVING ),
                                     phys::GetPhysics()->GetSystem()->GetDefaultLayerFilter( phys::Layers::MOVING ),
                                     {},
                                     {},
                                     *phys::GetPhysics()->GetTempAllocator() );
    }

    void Draw() const
    {
        if ( m_thirdPerson )
        {
            //const auto com = m_character->GetCenterOfMassTransform();
            //m_character->GetShape()->Draw( JPH::DebugRenderer::sInstance, com, JPH::Vec3::sOne(), JPH::Color::sGreen,
            //                               false, true );
        }

        // Gizmos::Box( HitPos, glm::quat(), glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.1f, 0.1f, 0.1f ) );
        // Gizmos::Line( StartPos, HitPos );
    }

    render::Camera* m_Camera = nullptr;

private:
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;

    bool m_thirdPerson = false;

    JPH::Ref<JPH::CharacterVirtual> m_character;

    float m_jumpBufferRemaining = 0.0f;
    float m_coyoteTimeRemaining = 0.0f;

    glm::vec3 m_moveInput = {};
    glm::vec3 m_previousCharacterPosition = {};
    glm::vec3 m_currentCharacterPosition = {};
};

extern Player* g_Player;

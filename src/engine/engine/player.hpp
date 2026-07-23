#pragma once

#include <algorithm>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#include "engine.hpp"
#include "window.hpp"
#include "common/transformable.hpp"
#include "input/iinput.hpp"
#include "physics/physics.hpp"
#include "physics/physics_renderer.hpp"
#include "renderer/gizmos.hpp"
#include "renderer/rendercamera.hpp"

constexpr auto SENSITIVITY = 10.0f;
constexpr auto CHARACTER_RADIUS = 0.5f;
constexpr auto CHARACTER_HEIGHT = 1.7f;
constexpr auto CHARACTER_EYE_HEIGHT = 1.65f;
constexpr auto CHARACTER_MAX_SPEED = 7.0f;

class Player
{
public:
    explicit Player( RenderCamera* camera )
        : m_Camera( camera )
    {
        const JPH::Ref settings = new JPH::CharacterVirtualSettings();
        settings->mShape = new JPH::BoxShape( JPH::Vec3( CHARACTER_RADIUS, 0.5f * CHARACTER_HEIGHT + CHARACTER_RADIUS,
                                                         CHARACTER_RADIUS ) );

        m_character = new JPH::CharacterVirtual( settings, JPH::RVec3( 0.0f, 1.0f, 0.0f ), JPH::Quat::sIdentity(),
                                                 GetPhysics()->GetSystem() );
        m_character->SetUp( JPH::Vec3( 0, 1, 0 ) );
    }

    ~Player() = default;

    void UpdateCamera( const float dt )
    {
        const auto pos = Utils::GLM_Vec3_JPH( m_character->GetPosition() ) -
                         glm::vec3( 0, ( CHARACTER_HEIGHT / 2 ), 0 );
        auto eyePos = pos + glm::vec3( 0, CHARACTER_EYE_HEIGHT, 0 );

        const auto delta = g_Input->GetMouseDelta();
        if ( g_Input->GetRelativeMouseMode() && g_Engine->GetWindow()->IsInputFocused() )
        {
            m_yaw += delta.x * SENSITIVITY * dt;
            m_pitch += delta.y * SENSITIVITY * dt;
            m_pitch = std::clamp( m_pitch, -85.0f, 85.0f );

            glm::quat qPitch = glm::angleAxis( glm::radians( m_pitch ), glm::vec3( 1, 0, 0 ) );
            glm::quat qYaw = glm::angleAxis( glm::radians( m_yaw ), glm::vec3( 0, 1, 0 ) );
            m_Camera->SetRotation( qYaw * qPitch );
        }

        if ( g_Input->IsKeyPressed( SDL_SCANCODE_C ) )
            m_thirdPerson = !m_thirdPerson;

        auto eyeForward = m_Camera->Forward();
        if ( m_thirdPerson )
            eyePos += eyeForward * -3.0f;

        m_Camera->SetPosition( eyePos );
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

    float GetFriction() const
    {
        if ( !IsOnGround() )
            return 1.2f;

        return 8.0f;
    }

    static float GetWishSpeed()
    {
        return CHARACTER_MAX_SPEED;
    }

    glm::vec3 StartPos = {};
    glm::vec3 HitPos = {};

    void UpdateMovement( const float dt )
    {
        m_character->UpdateGroundVelocity();

        const auto up = Utils::GLM_Vec3_JPH( m_character->GetUp() );
        auto currentVelocity = Utils::GLM_Vec3_JPH( m_character->GetLinearVelocity() );
        const auto currentVerticalVelocity = Utils::GLM_Vec3_JPH(
            m_character->GetLinearVelocity().Dot( m_character->GetUp() ) * m_character->GetUp() );
        const auto groundVelocity = Utils::GLM_Vec3_JPH( m_character->GetGroundVelocity() );
        glm::vec3 vel;

        m_wishVelocity = GetMovementDir();
        m_wishVelocity *= GetWishSpeed();

        if ( IsOnGround() )
        {
            vel = groundVelocity;

            if ( g_Input->IsKeyPressed( SDL_SCANCODE_SPACE ) )
                vel += 8.0f * up;
        } else
            vel = currentVerticalVelocity;

        //JPH::RayCast ray;
        //ray.mOrigin = JPH::Vec3(10, 2, 0);
        //ray.mDirection = JPH::Vec3(0, -2, 0);

        auto characterPos = Utils::GLM_Vec3_JPH( m_character->GetPosition() );
        StartPos = m_Camera->GetPosition();

        JPH::RayCastResult hit;
        JPH::RRayCast ray(
            Utils::JPH_Vec3_GLM( StartPos ),
            Utils::JPH_Vec3_GLM( m_Camera->Forward() ) * 5
        );

        Raycast( GetPhysics()->GetSystem(), ray, hit );

        JPH::RVec3 hitPosition = ray.GetPointOnRay( hit.mFraction );
        JPH::BodyID bodyID = hit.mBodyID;
        HitPos = Utils::GLM_Vec3_JPH( hitPosition );

        vel += m_wishVelocity;
        vel += g_Physics->GetGravity() * dt * 2.0f;

        //std::cout << m_wishVelocity.x << " " << m_wishVelocity.y << " " << m_wishVelocity.z << std::endl;

        m_character->SetLinearVelocity( Utils::JPH_Vec3_GLM( vel ) );
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

    void Update()
    {
        const float dt = g_Engine->DeltaTime();

        UpdateCamera( dt );
        UpdateMovement( dt );
        UpdatePhysics( dt );
    }

    void UpdatePhysics( const float dt ) const
    {
        const JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;
        m_character->ExtendedUpdate( dt,
                                     g_Physics->GetGravity().length() * -m_character->GetUp(),
                                     updateSettings,
                                     GetPhysics()->GetSystem()->GetDefaultBroadPhaseLayerFilter( Layers::MOVING ),
                                     GetPhysics()->GetSystem()->GetDefaultLayerFilter( Layers::MOVING ),
                                     {},
                                     {},
                                     *GetPhysics()->GetTempAllocator() );
    }

    void Draw() const
    {
        if ( m_thirdPerson )
        {
            const auto com = m_character->GetCenterOfMassTransform();
            m_character->GetShape()->Draw( JPH::DebugRenderer::sInstance, com, JPH::Vec3::sOne(), JPH::Color::sGreen,
                                           false, true );
        }

        Gizmos::Box( HitPos, glm::quat(), glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.1f, 0.1f, 0.1f ) );
        Gizmos::Line( StartPos, HitPos );
    }

    RenderCamera* m_Camera = nullptr;

private:
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;

    bool m_thirdPerson = false;

    JPH::Shape* m_standingShape = nullptr;
    JPH::CharacterVirtual* m_character = nullptr;

    glm::vec3 m_wishVelocity;
};

extern Player* g_Player;

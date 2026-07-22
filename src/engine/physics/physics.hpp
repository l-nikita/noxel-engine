#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "iphysics.hpp"
#include "physics_layers.hpp"
#include "physics_listeners.hpp"
#include "utils.hpp"

class PhysicsDebugRenderer;

class JoltPhysics final : public IPhysics
{
public:
    SysInit Initialize() override;
    void Shutdown() override;
    void Update() override;
    void Render() override;

    bool IsRunning() override
    {
        return m_isRunning;
    }

    glm::vec3 GetGravity() const override
    {
        return Utils::GLM_Vec3_JPH( m_system->GetGravity() );
    }

    JPH::PhysicsSystem* GetSystem() const
    {
        return m_system;
    }

    //static void UpdateEntityPhysics(entt::registry& registry);

    void MarkBodyForDestruction( JPH::BodyID bodyID );
    void FlushPendingDestructions() override;

    // Shapes
    // TODO: Make structs instead of arguments

    static JPH::BodyID CreateShapeBox(
        const JPH::RVec3& pos,
        const JPH::Quat& quat,
        const JPH::RVec3& offset,
        const JPH::Quat& quatOffset,
        const JPH::BoxShapeSettings* settings,
        JPH::EActivation activation = JPH::EActivation::Activate,
        JPH::EMotionType motionType = JPH::EMotionType::Dynamic );

    static JPH::BodyID CreateShapeSphere(
        const JPH::RVec3& pos,
        const JPH::Quat& quat,
        const JPH::RVec3& offset,
        const JPH::Quat& quatOffset,
        const JPH::SphereShapeSettings* settings,
        JPH::EActivation activation = JPH::EActivation::Activate,
        JPH::EMotionType motionType = JPH::EMotionType::Dynamic );

    std::vector<JPH::BodyID> GetBodiesIDs()
    {
        return m_bodiesIDs;
    }

    JPH::TempAllocatorImpl* GetTempAllocator() const
    {
        return m_tempAllocator;
    }

private:
    bool m_isRunning = false;

    PhysicsDebugRenderer* m_debugRenderer = nullptr;

    std::vector<JPH::BodyID> m_bodiesIDs;
    std::vector<JPH::BodyID> m_bodiesToDestroy;

    JPH::PhysicsSystem* m_system = nullptr;
    JPH::TempAllocatorImpl* m_tempAllocator = nullptr;
    JPH::JobSystemThreadPool* m_jobSystem = nullptr;

    BPLayerInterfaceImpl* broad_phase_layer_interface = nullptr;
    ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter = nullptr;
    ObjectLayerPairFilterImpl* object_vs_object_layer_filter = nullptr;

    MyBodyActivationListener* body_activation_listener = nullptr;
    MyContactListener* contact_listener = nullptr;
};

// not good
static JoltPhysics* GetPhysics()
{
    return static_cast<JoltPhysics*>(g_Physics);
}

struct EPhysicsBody
{
    JPH::BodyID BodyID;

    explicit EPhysicsBody( const JPH::BodyID id ) : BodyID( id )
    {
    }

    ~EPhysicsBody() // Calls 2 times ?
    {
        GetPhysics()->MarkBodyForDestruction( BodyID );
    }
};

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "common/singleton.hpp"
#include "dbg/profiler.hpp"

#include "iphysics.hpp"
#include "physics_layers.hpp"
#include "physics_listeners.hpp"
#include "physics_renderer.hpp"

class JoltPhysics : public IPhysics
{
public:
	~JoltPhysics() { Shutdown(); }

	void Init() override;
	void Update() override;
	void Render() override;
	void Shutdown() override;

	bool IsRunning() override { return m_isRunning; }

	glm::vec3 GetGravity() const override { return Utils::GLM_Vec3_JPH(m_system->GetGravity()); }

	JPH::PhysicsSystem* GetSystem() { return m_system; }

	//static void UpdateEntityPhysics(entt::registry& registry);

	void MarkBodyForDestruction(JPH::BodyID bodyID);
	void FlushPendingDestructions() override;

	// Shapes
	// TODO: Make structs instead of arguments

	static JPH::BodyID CreateShapeBox(
		const JPH::RVec3& pos,
		const JPH::Quat& quat,
		const JPH::RVec3& offset,
		const JPH::Quat& quatOffset,
		JPH::BoxShapeSettings* settings,
		JPH::EActivation activation = JPH::EActivation::Activate,
		JPH::EMotionType motionType = JPH::EMotionType::Dynamic);

	static JPH::BodyID CreateShapeSphere(
		const JPH::RVec3& pos,
		const JPH::Quat& quat,
		const JPH::RVec3& offset,
		const JPH::Quat& quatOffset,
		JPH::SphereShapeSettings* settings,
		JPH::EActivation activation = JPH::EActivation::Activate,
		JPH::EMotionType motionType = JPH::EMotionType::Dynamic);

	std::vector<JPH::BodyID> GetBodiesIDs() { return m_bodiesIDs; }

	JPH::TempAllocatorImpl* GetTempAllocator() { return m_tempAllocator; }

private:
	bool m_isRunning;

	PhysicsDebugRenderer* m_debugRenderer;

	std::vector<JPH::BodyID> m_bodiesIDs;
	std::vector<JPH::BodyID> m_bodiesToDestroy;

	JPH::PhysicsSystem* m_system;
	JPH::TempAllocatorImpl* m_tempAllocator;
	JPH::JobSystemThreadPool* m_jobSystem;

	BPLayerInterfaceImpl* broad_phase_layer_interface;
	ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter;
	ObjectLayerPairFilterImpl* object_vs_object_layer_filter;

	MyBodyActivationListener* body_activation_listener;
	MyContactListener* contact_listener;
};

// not good
static JoltPhysics* GetPhysics()
{
	return static_cast<JoltPhysics*>(g_Physics);
}

struct EPhysicsBody
{
	JPH::BodyID BodyID;

	EPhysicsBody(JPH::BodyID id) : BodyID(id) {}

	~EPhysicsBody() // Calls 2 times ?
	{
		GetPhysics()->MarkBodyForDestruction(BodyID);
	}
};
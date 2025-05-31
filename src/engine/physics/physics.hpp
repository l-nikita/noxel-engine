#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include "singleton.hpp"
#include "profiler.hpp"
#include "entities.hpp"

#include "physics_layers.hpp"
#include "physics_listeners.hpp"

struct EPhysicsBody
{
	JPH::Body* Body = nullptr;
};

class Physics : public Singleton<Physics>
{
public:
	~Physics() { Shutdown(); }

	bool Init();
	void Update();
	void Shutdown();

	bool IsRunning() { return m_IsRunning; }
	Profiler* m_updateProfiler;

	JPH::PhysicsSystem* GetSystem() { return m_system; }

	static void UpdateEntityPhysics(entt::registry& registry);

	// Shapes

	static JPH::Body* CreateShapeBox(
		const JPH::RVec3& pos,
		const JPH::Quat& quat,
		const JPH::RVec3& offset,
		const JPH::Quat& quatOffset,
		const JPH::RVec3& halfExtent,
		const float& convexRadius,
		JPH::EMotionType motionType = JPH::EMotionType::Dynamic,
		const JPH::PhysicsMaterial* material = nullptr);

private:
	bool m_IsRunning;

	JPH::PhysicsSystem* m_system;
	JPH::TempAllocatorImpl* m_tempAllocator;
	JPH::JobSystemThreadPool* m_jobSystem;

	BPLayerInterfaceImpl* broad_phase_layer_interface;
	ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter;
	ObjectLayerPairFilterImpl* object_vs_object_layer_filter;

	MyBodyActivationListener* body_activation_listener;
	MyContactListener* contact_listener;
};
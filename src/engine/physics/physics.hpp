#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "singleton.hpp"
#include "physics_layers.hpp"
#include "physics_listeners.hpp"

class Physics : public Singleton<Physics>
{
public:
	~Physics() { Shutdown(); }

	bool Init();
	void Update();
	void Shutdown();

private:
	JPH::PhysicsSystem* m_system;
	JPH::TempAllocatorImpl* m_tempAllocator;
	JPH::JobSystemThreadPool* m_jobSystem;

	BPLayerInterfaceImpl* broad_phase_layer_interface;
	ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter;
	ObjectLayerPairFilterImpl* object_vs_object_layer_filter;

	MyBodyActivationListener* body_activation_listener;
	MyContactListener* contact_listener;
};
#include "physics.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <cstdarg>
#include <thread>

#include "engine.hpp"
#include "cmd.hpp"
#include "gizmos.hpp"

// TO DO: MAKE RENDERER

using namespace JPH;

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	CMD_LOG_INFO(buffer);
}

#ifdef JPH_ENABLE_ASSERTS

static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint inLine)
{
	std::string str = inFile + std::string(":") + std::to_string(inLine) + ": (" + inExpression + ") " + (inMessage != nullptr ? inMessage : "");
	CMD_LOG(str, ERROR);
	throw std::runtime_error(str);

	return true;
};

#endif // JPH_ENABLE_ASSERTS


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static Body* TestFloor;
static BodyID TestSphereID;

bool Physics::Init()
{
	m_updateProfiler = new Profiler("PhysicsUpdate");

	RegisterDefaultAllocator();

	Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

	Factory::sInstance = new Factory();
	RegisterTypes();

	m_tempAllocator = new TempAllocatorImpl(10 * 1024 * 1024);
	m_jobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	const uint cMaxBodies = 1024;
	const uint cNumBodyMutexes = 0;
	const uint cMaxBodyPairs = 1024;
	const uint cMaxContactConstraints = 1024;

	broad_phase_layer_interface = new BPLayerInterfaceImpl();
	object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
	object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();

	m_system = new PhysicsSystem();
	m_system->Init(cMaxBodies,
		cNumBodyMutexes, 
		cMaxBodyPairs, 
		cMaxContactConstraints, 
		*broad_phase_layer_interface,
		*object_vs_broadphase_layer_filter, 
		*object_vs_object_layer_filter);

	body_activation_listener = new MyBodyActivationListener();
	m_system->SetBodyActivationListener(body_activation_listener);

	contact_listener = new MyContactListener();
	m_system->SetContactListener(contact_listener);

	//---------------------------------------------------------------------
	BodyInterface& body_interface = m_system->GetBodyInterface();

	BoxShapeSettings floor_shape_settings(RVec3(10.0f, 1.0f, 10.0f));
	floor_shape_settings.SetEmbedded();

	ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	ShapeRefC floor_shape = floor_shape_result.Get();

	BodyCreationSettings floor_settings(floor_shape, RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);

	TestFloor = body_interface.CreateBody(floor_settings);
	body_interface.AddBody(TestFloor->GetID(), EActivation::DontActivate);

	//glm::vec3 center(-2.5f, -1.0f, -2.5f);
	//glm::vec3 half_extents(10.0f, 1.0f, 10.0f);

	//glm::vec3 min = center - half_extents;
	//glm::vec3 max = center + half_extents;

	//Gizmo::Box::Create(min, max, glm::vec3(1.0f));

	////---------------------------------------------------------------------
	//BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(-2.5f, 5.0f, -2.5f), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	//TestSphereID = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);

	//body_interface.SetLinearVelocity(TestSphereID, Vec3(0.0f, -5.0f, 0.0f));

	m_system->OptimizeBroadPhase();
	m_IsRunning = true;

	CMD_LOG("] Physics initialized", INFO);
    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Physics::Update()
{
	if (!IsRunning()) return;
	m_updateProfiler->Start();

	const float cDeltaTime = 1.0f / 144.0f;
	const int cCollisionSteps = 1;

	// Step the world
	m_system->Update(cDeltaTime, 1, m_tempAllocator, m_jobSystem);

	m_updateProfiler->End();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Physics::Shutdown()
{
	if (!IsRunning()) return;

	BodyInterface& body_interface = m_system->GetBodyInterface();
	body_interface.RemoveBody(TestFloor->GetID());
	body_interface.DestroyBody(TestFloor->GetID());

	UnregisterTypes();

	delete m_updateProfiler;

	delete m_system;
	delete m_tempAllocator;
	delete m_jobSystem;

	delete broad_phase_layer_interface;
	delete object_vs_broadphase_layer_filter;
	delete object_vs_object_layer_filter;

	delete body_activation_listener;
	delete contact_listener;

	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}
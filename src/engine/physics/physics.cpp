#include "physics.hpp"
#include "physics_renderer.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include <cstdarg>
#include <iostream>
#include <string>
#include <thread>
#include <glm/glm.hpp>

#include "engine.hpp"
#include "dbg/debug.hpp"

namespace phys
{
    using namespace JPH;

    // Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
    JPH_SUPPRESS_WARNINGS

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    static void TraceImpl( const char* inFMT, ... )
    {
        // Format the message
        va_list list;
        va_start( list, inFMT );
        char buffer[ 1024 ];
        vsnprintf( buffer, sizeof( buffer ), inFMT, list );
        va_end( list );

        CMD_INFO( buffer, cmd::PHYSICS );
    }

#ifdef JPH_ENABLE_ASSERTS

    static bool AssertFailedImpl( const char* inExpression, const char* inMessage, const char* inFile, uint inLine )
    {
        auto str = inFile + std::string( ":" ) + std::to_string( inLine ) + ": (" + inExpression + ") " + (
                       inMessage != nullptr ? inMessage : "" );
        FATAL_ERROR( str, cmd::PHYSICS );

        return true;
    };

#endif // JPH_ENABLE_ASSERTS

    static BodyID g_BodyId;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    SysInit JoltPhysics::Initialize()
    {
        RegisterDefaultAllocator();

        Trace = TraceImpl;
        JPH_IF_ENABLE_ASSERTS( AssertFailed = AssertFailedImpl;
        )

        Factory::sInstance = new Factory();
        RegisterTypes();

        m_debugRenderer = new PhysicsDebugRenderer();

        const auto hardwareThreads = std::thread::hardware_concurrency();
        const auto workerThreads = hardwareThreads > 1 ? hardwareThreads - 1 : 1;

        m_tempAllocator = new TempAllocatorImpl( 10 * 1024 * 1024 );
        m_jobSystem = new JobSystemThreadPool( cMaxPhysicsJobs, cMaxPhysicsBarriers, workerThreads );

        constexpr uint cMaxBodies = 1024;
        constexpr uint cNumBodyMutexes = 0;
        constexpr uint cMaxBodyPairs = 1024;
        constexpr uint cMaxContactConstraints = 1024;

        broad_phase_layer_interface = new BPLayerInterfaceImpl();
        object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
        object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();

        m_system = new PhysicsSystem();
        m_system->Init( cMaxBodies,
                        cNumBodyMutexes,
                        cMaxBodyPairs,
                        cMaxContactConstraints,
                        *broad_phase_layer_interface,
                        *object_vs_broadphase_layer_filter,
                        *object_vs_object_layer_filter );

        body_activation_listener = new MyBodyActivationListener();
        m_system->SetBodyActivationListener( body_activation_listener );

        contact_listener = new MyContactListener();
        m_system->SetContactListener( contact_listener );

        //---------------------------------------------------------------------
        JoltPhysics::CreateShapeBox(
            RVec3( 0.0f, -5.0f, 0.0f ), Quat::sIdentity(),
            RVec3( 0.0f, 0.0f, 0.0f ), Quat::sIdentity(),
            new JPH::BoxShapeSettings( JPH::RVec3( 20.0f, 1.0f, 20.0f ) ),
            EActivation::Activate, EMotionType::Static );

        JoltPhysics::CreateShapeBox(
            RVec3( 5.0f, -3.0f, 10.0f ), Quat::sIdentity(),
            RVec3( 0.0f, 0.0f, 0.0f ), Quat::sIdentity(),
            new JPH::BoxShapeSettings( JPH::RVec3( 3.0f, 1.0f, 3.0f ) ),
            EActivation::Activate, EMotionType::Static, Layers::NON_MOVING );

        //---------------------------------------------------------------------
        JoltPhysics::CreateShapeBox(
            RVec3( 0, -3, -5 ), Quat::sIdentity(),
            RVec3( 0.0f, 0.0f, 0.0f ), Quat::sIdentity(),
            new JPH::BoxShapeSettings( JPH::RVec3( 1.0f, 1.0f, 1.0f ) ),
            EActivation::Activate, EMotionType::Static, Layers::NON_MOVING );

        JoltPhysics::CreateShapeBox(
            RVec3( 0, -3, 15 ), Quat::sIdentity(),
            RVec3( 0.0f, 0.0f, 0.0f ), Quat::sIdentity(),
            new JPH::BoxShapeSettings( JPH::RVec3( 1.0f, 1.0f, 1.0f ) ),
            EActivation::Activate, EMotionType::Static, Layers::NON_MOVING );

        JoltPhysics::CreateShapeBox(
            RVec3( -5, -3, -2 ), Quat::sIdentity(),
            RVec3( 0.0f, 0.0f, 0.0f ), Quat::sIdentity(),
            new JPH::BoxShapeSettings( JPH::RVec3( 1.0f, 1.0f, 1.0f ) ),
            EActivation::Activate, EMotionType::Static, Layers::NON_MOVING );

        g_BodyId = JoltPhysics::CreateShapeBox(
            RVec3( 0, 0, 0 ), Quat::sIdentity(),
            RVec3( 0.0f, 0.0f, 0.0f ), Quat::sIdentity(),
            new JPH::BoxShapeSettings( JPH::RVec3( 0.5f, 0.5f, 0.5f ) ),
            EActivation::Activate, EMotionType::Static, Layers::MOVING );

        //---------------------------------------------------------------------
        m_system->OptimizeBroadPhase();
        m_isRunning = true;

        return {};
    }

    void JoltPhysics::Shutdown()
    {
        m_isRunning = false;

        FlushPendingDestructions();

        auto& iBody = m_system->GetBodyInterface();
        for ( auto& bodyID: m_bodiesIDs )
        {
            iBody.RemoveBody( bodyID );
            iBody.DestroyBody( bodyID );
        }

        m_bodiesIDs.clear();
        UnregisterTypes();

        delete m_system;
        delete m_tempAllocator;
        delete m_jobSystem;

        delete Factory::sInstance;
        Factory::sInstance = nullptr;

        delete broad_phase_layer_interface;
        delete object_vs_broadphase_layer_filter;
        delete object_vs_object_layer_filter;

        delete body_activation_listener;
        delete contact_listener;

        delete m_debugRenderer;
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void JoltPhysics::Update( const float dt )
    {
        if ( !IsRunning() ) return;

        PROF_START( "Physics::Update", "Physics" );

        constexpr auto cCollisionSteps = 1;

        auto& iBody = m_system->GetBodyInterface();

        static auto totalTime = 0.0f;
        totalTime += g_Engine->DeltaTime();

        iBody.SetPosition( g_BodyId, Vec3(
                               sin( totalTime ) * 5.0f,
                               -3.0f,
                               cos( totalTime ) * 5.0f
                           ), EActivation::Activate );

        const auto rot = glm::angleAxis( totalTime * 3.0f, glm::normalize( glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
        iBody.SetRotation( g_BodyId, utils::JPH_Quat_GLM( rot ), JPH::EActivation::Activate );

        // Step the world
        m_system->Update( dt, cCollisionSteps, m_tempAllocator, m_jobSystem );
        FlushPendingDestructions();

        PROF_END( "Physics::Update" );
    }

    void JoltPhysics::Render()
    {
        if ( !IsRunning() ) return;

        PROF_START( "Physics::Render", "Physics" );

        BodyManager::DrawSettings drawSettings;
        drawSettings.mDrawShape = false;
        drawSettings.mDrawBoundingBox = true;
        drawSettings.mDrawCenterOfMassTransform = true;
        m_system->DrawBodies( drawSettings, m_debugRenderer );

        PROF_END( "Physics::Render" );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void JoltPhysics::MarkBodyForDestruction( const BodyID bodyID )
    {
        for ( auto& bodyToDestroyID: m_bodiesToDestroy )
            if ( bodyToDestroyID == bodyID )
                return;

        m_bodiesToDestroy.push_back( bodyID );
    }

    void JoltPhysics::FlushPendingDestructions()
    {
        auto& iBody = m_system->GetBodyInterface();
        for ( auto& bodyID: m_bodiesToDestroy )
        {
            iBody.RemoveBody( bodyID );
            iBody.DestroyBody( bodyID );

            if ( auto it = std::ranges::remove( m_bodiesIDs, bodyID ).begin(); it != m_bodiesIDs.end() )
                m_bodiesIDs.erase( it, m_bodiesIDs.end() );
        }

        m_bodiesToDestroy.clear();
    }
}

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace phys
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class MyBodyActivationListener final : public JPH::BodyActivationListener
    {
    public:
        void OnBodyActivated( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData ) override
        {
            //CMD_LOG_INFO("A body got activated");
        }

        void OnBodyDeactivated( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData ) override
        {
            //CMD_LOG_INFO("A body went to sleep");
        }
    };

    class MyContactListener final : public JPH::ContactListener
    {
    public:
        // See: ContactListener
        JPH::ValidateResult OnContactValidate( const JPH::Body& inBody1, const JPH::Body& inBody2,
                                               JPH::RVec3Arg inBaseOffset,
                                               const JPH::CollideShapeResult& inCollisionResult ) override
        {
            //CMD_LOG_INFO("Contact validate callback");

            // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
            return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        void OnContactAdded( const JPH::Body& inBody1, const JPH::Body& inBody2,
                             const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings ) override
        {
            //CMD_LOG_INFO("A contact was added");
        }

        void OnContactPersisted( const JPH::Body& inBody1, const JPH::Body& inBody2,
                                 const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings ) override
        {
            //CMD_LOG_INFO("A contact was persisted");
        }

        void OnContactRemoved( const JPH::SubShapeIDPair& inSubShapePair ) override
        {
            //CMD_LOG_INFO("A contact was removed");
        }
    };
}

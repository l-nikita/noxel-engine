#include "physics.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Body/BodyInterface.h>

using namespace JPH;

BodyID Physics::CreateShapeBox(
    const RVec3& pos, 
    const Quat& quat, 
    const RVec3& offset, 
    const Quat& quatOffset, 
    const RVec3& halfExtent, 
    EActivation activation,
    EMotionType motionType,
    float convexRadius,
    const PhysicsMaterial* material)
{
    auto settings = new BoxShapeSettings(halfExtent, convexRadius, material);
    settings->mDensity = 10.0f; 

    RotatedTranslatedShapeSettings offsetShapeSettings(offset, quat, settings);
    ShapeSettings::ShapeResult result = offsetShapeSettings.Create();

    BodyInterface& body_interface = GetInstance().GetSystem()->GetBodyInterface();
    BodyCreationSettings bodySettings(result.Get(), pos, quatOffset, motionType, Layers::MOVING);

    auto bodyID = body_interface.CreateAndAddBody(bodySettings, activation);
    GetInstance().m_bodiesIDs.push_back(bodyID);
    return bodyID;
}

BodyID Physics::CreateShapeSphere(
    const RVec3& pos, 
    const Quat& quat, 
    const RVec3& offset, 
    const Quat& quatOffset, 
    float radius,
    EActivation activation,
    EMotionType motionType,
    float convexRadius,
    const PhysicsMaterial* material)
{
    auto settings = new SphereShapeSettings(radius, material);
    settings->mDensity = 10.0f; 

    RotatedTranslatedShapeSettings offsetShapeSettings(offset, quat, settings);
    ShapeSettings::ShapeResult result = offsetShapeSettings.Create();

    BodyInterface& body_interface = GetInstance().GetSystem()->GetBodyInterface();
    BodyCreationSettings bodySettings(result.Get(), pos, quatOffset, motionType, Layers::MOVING);

    auto bodyID = body_interface.CreateAndAddBody(bodySettings, activation);
    GetInstance().m_bodiesIDs.push_back(bodyID);
    return bodyID;
}
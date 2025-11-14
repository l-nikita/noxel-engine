#include "physics.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Body/BodyInterface.h>

using namespace JPH;

BodyID JoltPhysics::CreateShapeBox(
    const RVec3& pos, 
    const Quat& quat, 
    const RVec3& offset, 
    const Quat& quatOffset, 
    JPH::BoxShapeSettings* settings,
    EActivation activation,
    EMotionType motionType)
{
    RotatedTranslatedShapeSettings offsetShapeSettings(offset, quat, settings);
    ShapeSettings::ShapeResult result = offsetShapeSettings.Create();

    BodyInterface& body_interface = GetPhysics()->GetSystem()->GetBodyInterface();
    BodyCreationSettings bodySettings(result.Get(), pos, quatOffset, motionType, Layers::MOVING);

    auto bodyID = body_interface.CreateAndAddBody(bodySettings, activation);
    GetPhysics()->m_bodiesIDs.push_back(bodyID);
    return bodyID;
}

BodyID JoltPhysics::CreateShapeSphere(
    const RVec3& pos, 
    const Quat& quat, 
    const RVec3& offset, 
    const Quat& quatOffset, 
    JPH::SphereShapeSettings* settings,
    EActivation activation,
    EMotionType motionType)
{
    RotatedTranslatedShapeSettings offsetShapeSettings(offset, quat, settings);
    ShapeSettings::ShapeResult result = offsetShapeSettings.Create();

    BodyInterface& body_interface = GetPhysics()->GetSystem()->GetBodyInterface();
    BodyCreationSettings bodySettings(result.Get(), pos, quatOffset, motionType, Layers::MOVING);

    auto bodyID = body_interface.CreateAndAddBody(bodySettings, activation);
    GetPhysics()->m_bodiesIDs.push_back(bodyID);
    return bodyID;
}
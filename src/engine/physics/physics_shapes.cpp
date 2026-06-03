#include "physics.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

using namespace JPH;

BodyID JoltPhysics::CreateShapeBox(
    const RVec3& pos,
    const Quat& quat,
    const RVec3& offset,
    const Quat& quatOffset,
    const BoxShapeSettings* settings,
    const EActivation activation,
    const EMotionType motionType )
{
    const RotatedTranslatedShapeSettings offsetShapeSettings(offset, quat, settings);
    const auto result = offsetShapeSettings.Create();

    auto& body_interface = GetPhysics()->GetSystem()->GetBodyInterface();
    const BodyCreationSettings bodySettings(result.Get(), pos, quatOffset, motionType, Layers::MOVING);

    const auto bodyID = body_interface.CreateAndAddBody(bodySettings, activation);
    GetPhysics()->m_bodiesIDs.push_back(bodyID);
    return bodyID;
}

BodyID JoltPhysics::CreateShapeSphere(
    const RVec3& pos,
    const Quat& quat,
    const RVec3& offset,
    const Quat& quatOffset,
    const SphereShapeSettings* settings,
    const EActivation activation,
    const EMotionType motionType )
{
    const RotatedTranslatedShapeSettings offsetShapeSettings(offset, quat, settings);
    const auto result = offsetShapeSettings.Create();

    auto& body_interface = GetPhysics()->GetSystem()->GetBodyInterface();
    const BodyCreationSettings bodySettings(result.Get(), pos, quatOffset, motionType, Layers::MOVING);

    const auto bodyID = body_interface.CreateAndAddBody(bodySettings, activation);
    GetPhysics()->m_bodiesIDs.push_back(bodyID);
    return bodyID;
}

#pragma once

#include "entities.hpp"
#include "physics/physics.hpp"

struct PlayerInput 
{
    float MoveForward;
    float MoveRight;
    bool Jump;
};

static Entity CreatePlayer(int shape = 0) 
{
    auto entity = Entities::GetInstance().CreateEntity();
    auto& transform = entity.Add<Transform>();
    transform.Position = glm::vec3(0.0f, 5.0f, 0.0f);

    if (shape == 1)
    {
        auto settings = new JPH::SphereShapeSettings(1.0f);
        settings->mDensity = 20;

        auto body = Physics::CreateShapeSphere(
            Utils::JPH_Vec3_GLM(transform.Position), Utils::JPH_Quat_GLM(transform.Rotation),
            JPH::RVec3(0.0f, 0.0f, 0.0f), JPH::Quat::sIdentity(), settings);

        entity.Add<EPhysicsBody>(body);
    }
    else
    {
        auto settings = new JPH::BoxShapeSettings(JPH::RVec3(1.0f, 1.0f, 1.0f));
        settings->mDensity = 20;

        auto body = Physics::CreateShapeBox(
            Utils::JPH_Vec3_GLM(transform.Position), Utils::JPH_Quat_GLM(transform.Rotation),
            JPH::RVec3(0.0f, 0.0f, 0.0f), JPH::Quat::sIdentity(), settings);

        entity.Add<EPhysicsBody>(body);
    }

    entity.Add<PlayerInput>();

    return entity;
}
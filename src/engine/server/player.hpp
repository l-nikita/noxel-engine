#pragma once

#include "entities.hpp"
#include "physics.hpp"

struct PlayerInput 
{
    float MoveForward;
    float MoveRight;
    bool Jump;
};

static Entity CreatePlayer(entt::registry& registry) 
{
    auto entity = registry.create();
    auto& transform = registry.emplace<Transform>(entity);
    transform.Position = glm::vec3(0.0f, 5.0f, 0.0f);

    auto body = Physics::CreateShapeBox(
        Utils::JPH_Vec3_GLM(transform.Position), Utils::JPH_Quat_GLM(transform.Rotation),
        JPH::RVec3(0.0f, 0.0f, 0.0f), JPH::Quat::sIdentity(),
        JPH::RVec3(1.0f, 1.0f, 1.0f));

    registry.emplace<EPhysicsBody>(entity, body);
    registry.emplace<PlayerInput>(entity);

    return Entity(registry, entity);
}
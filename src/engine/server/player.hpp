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

    //Physics::CreateShapeBox(tr)

    registry.emplace<EPhysicsBody>(entity, body);
    registry.emplace<PlayerInput>(entity);

    return Entity(registry, entity);
}
#include "physics.hpp"
#include "entities.hpp"
#include "input.hpp"

void Physics::UpdateEntityPhysics(entt::registry& registry)
{
    JPH::BodyInterface& body_interface = GetInstance().GetSystem()->GetBodyInterface();

    auto view = registry.view<EPhysicsBody, Transform>();
    for (auto [entity, physics, transform] : view.each())
    {
        JPH::BodyID bodyID = physics.BodyID;
        
        transform.Position = Utils::GLM_Vec3_JPH(body_interface.GetPosition(bodyID));
        transform.Rotation = Utils::GLM_Quat_JPH(body_interface.GetRotation(bodyID));

        if (Input::GetInstance().IsKeyPressed(SDL_SCANCODE_R))
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(-90, 90);
            std::uniform_int_distribution<> dist2(-200, 200);

            body_interface.SetPosition(bodyID, JPH::RVec3(0.0f, 5.0f, 0.0f), JPH::EActivation::Activate);
            body_interface.SetRotation(bodyID, JPH::Quat::sEulerAngles(JPH::RVec3(dist(gen), dist(gen), dist(gen))), JPH::EActivation::Activate);
            body_interface.SetLinearVelocity(bodyID, JPH::RVec3(0.0f, 0.0f, 0.0f));

            body_interface.AddImpulse(bodyID, JPH::RVec3(dist2(gen), 500.0f, dist2(gen)));
            body_interface.AddAngularImpulse(bodyID, JPH::RVec3(dist2(gen), dist2(gen), dist2(gen)));
        }
    }
}
#include "physics.hpp"
#include "entities.hpp"
#include "input.hpp"

void Physics::UpdateEntityPhysics(entt::registry& registry)
{
    JPH::BodyInterface& body_interface = GetInstance().GetSystem()->GetBodyInterface();

    auto view = registry.view<EPhysicsBody, Transform>();
    for (auto [entity, physics, transform] : view.each())
    {
        JPH::Body* body = physics.Body;

        auto pos = body->GetPosition();
        auto rot = body->GetRotation();

        transform.Position = glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
        transform.Rotation = glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());

        if (Input::GetInstance().IsKeyPressed(SDL_SCANCODE_R))
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(-90, 90);
            std::uniform_int_distribution<> dist2(-500, 500);

            body_interface.SetPosition(body->GetID(), JPH::RVec3(0.0f, 5.0f, 0.0f), JPH::EActivation::Activate);
            body_interface.SetRotation(body->GetID(), JPH::Quat::sEulerAngles(JPH::RVec3(dist(gen), dist(gen), dist(gen))), JPH::EActivation::Activate);
            body_interface.SetLinearVelocity(body->GetID(), JPH::RVec3(0.0f, 0.0f, 0.0f));

            body_interface.AddImpulse(body->GetID(), JPH::RVec3(dist2(gen), 500.0f, dist2(gen)));
            body_interface.AddAngularImpulse(body->GetID(), JPH::RVec3(dist2(gen), dist2(gen), dist2(gen)));
        }
    }
}
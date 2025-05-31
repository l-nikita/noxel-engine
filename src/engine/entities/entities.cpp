#include <random>

#include "entities.hpp"
#include "cmd.hpp"
#include "model.hpp"
#include "physics.hpp"
#include "player.hpp"
#include "gizmos.hpp"
#include "input.hpp"

static std::vector<Entity> Players;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Entities::Init()
{
    m_isRunning = true;

    //for (int i = 0; i < 25; i++)
    //{
    //    Entity entity = Entities::CreateEntity();
    //    entity.Add<Transform>();
    //    entity.Add<EModel>(model);

    //    entity.Get<Transform>().Position = glm::vec3(i * 1.0f, 0.0f, 0.0f);
    //}

    for (int i = 0; i < 10; i++)
    {
        Players.push_back(CreatePlayer(m_registry));
    }

    CMD_LOG("] Entities initialized", INFO);
    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Entities::Update()
{
    if (!IsRunning()) return;

    Physics::UpdateEntityPhysics(m_registry);
}

void Entities::DrawAll()
{
    if (!IsRunning()) return;

    auto view = m_registry.view<Transform, EModel>();
    for (auto [entity, transform, renderable] : view.each())
    {
        auto& model = renderable.Model;

        if (!model) 
            continue;

        model->SetTransform(transform);
        model->Draw();
    }

    for (auto& player : Players)
    {
        auto& playerTrans = player.Get<Transform>();
        Gizmo::DrawBox(glm::vec3(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(-10.0f, -1.0f, -10.0f));
        Gizmo::DrawBox(playerTrans.Position, playerTrans.Rotation, glm::vec3(1.0f), glm::vec3(-1.0f));
        Gizmo::DrawLine(glm::vec3(), playerTrans.Position);
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Entities::Shutdown()
{
    m_registry.clear();
    m_UUIDs.clear();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Entity Entities::CreateEntity()
{
    auto& registry = GetInstance().m_registry;
    auto entity = registry.create();
    auto uuid = boost::uuids::random_generator()();

    registry.emplace<EEntityID>(entity, EEntityID{ uuid });
    GetInstance().m_UUIDs[uuid] = entity;
    return Entity(registry, entity);
}

std::optional<Entity> Entities::GetEntityByUUID(const UUID& uuid)
{
    auto& self = GetInstance();
    auto it = self.m_UUIDs.find(uuid);

    if (it != self.m_UUIDs.end())
        return Entity(self.m_registry, it->second);

    return std::nullopt;
}
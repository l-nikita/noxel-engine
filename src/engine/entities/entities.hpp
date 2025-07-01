#pragma once

#include <entt/entt.hpp>
#include <boost/uuid.hpp>

#include "ientities.hpp"
#include "common/singleton.hpp"
#include "common/transform.hpp"
#include "renderer/model.hpp"

//-----------------------------------------------------------------------------
struct EEntityID
{
    UUID ID;
};

struct EModel
{
    Model* Model;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Entity
{
public:
    Entity(entt::registry& registry, const entt::entity entity)
        : m_registry(registry), m_handler(entity)
    {
    }

    bool IsValid() const { return m_registry.valid(m_handler); }
    entt::entity GetHandler() const { return m_handler; }

    template<typename T>
    T& Get() { return m_registry.get<T>(m_handler); }

    template<typename T>
    bool Has() const { return m_registry.all_of<T>(m_handler); }

    template<typename T, typename... Args>
    T& Add(Args&&... args) { return m_registry.emplace<T>(m_handler, std::forward<Args>(args)...); }

    template<typename T>
    void Remove() { m_registry.remove<T>(m_handler); }

    void Destroy()
    {
        if (IsValid())
            m_registry.destroy(m_handler);
    }

    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

protected:
    const entt::entity m_handler;
    entt::registry& m_registry;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Entities : public IEntities
{
public:
    bool Init() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;
    void DestroyAll();

    Entity CreateEntity() override;
    std::optional<Entity> GetEntityByUUID(const UUID& uuid) override;

    entt::registry& GetRegistry() override { return m_registry; }

    bool IsRunning() override { return m_isRunning; }

private:
    bool m_isRunning;

    std::unordered_map<UUID, entt::entity> m_UUIDs;
    entt::registry m_registry;
};
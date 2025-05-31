#pragma once

#include <entt/entt.hpp>
#include <boost/uuid.hpp>

#include "singleton.hpp"
#include "transform.hpp"
#include "model.hpp"

//-----------------------------------------------------------------------------
typedef boost::uuids::uuid UUID;

struct EEntityID
{
    UUID ID;
};

struct EModel
{
    std::shared_ptr<Model> Model;
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

    void Destroy();

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
class Entities : public Singleton<Entities>
{
public:

    bool Init();
    void Update();
    void DrawAll();
    void Shutdown();

    static Entity CreateEntity();
    std::optional<Entity> GetEntityByUUID(const UUID& uuid);

    entt::registry& GetRegistry() { return m_registry; }

    bool IsRunning() { return m_isRunning; }

private:
    bool m_isRunning;

    std::unordered_map<UUID, entt::entity> m_UUIDs;
    entt::registry m_registry;
};
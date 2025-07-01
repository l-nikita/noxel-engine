#pragma once

#include <optional>
#include <entt/entt.hpp>
#include <boost/uuid.hpp>

typedef boost::uuids::uuid UUID;

class Entity;

class IEntities
{
public:
	virtual ~IEntities() = default;

    virtual bool Init() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;

    virtual bool IsRunning() = 0;

    virtual Entity CreateEntity() = 0;
    virtual std::optional<Entity> GetEntityByUUID(const UUID& uuid) = 0;

    virtual entt::registry& GetRegistry() = 0;
};

extern IEntities* g_Entities;
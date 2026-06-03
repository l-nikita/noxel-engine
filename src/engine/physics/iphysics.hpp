#pragma once

#include <glm/glm.hpp>

class IPhysics
{
public:
    virtual ~IPhysics() = default;

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;

    virtual bool IsRunning() = 0;

    virtual glm::vec3 GetGravity() const = 0;

    virtual void FlushPendingDestructions() = 0;
};

extern IPhysics* g_Physics;

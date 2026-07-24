#pragma once

#include <glm/glm.hpp>
#include "common.hpp"

class IPhysics : public ISystem
{
public:
    virtual void Update( float dt ) = 0;
    virtual void Render() = 0;

    virtual bool IsRunning() = 0;

    virtual glm::vec3 GetGravity() const = 0;

    virtual void FlushPendingDestructions() = 0;
};

extern IPhysics* g_Physics;

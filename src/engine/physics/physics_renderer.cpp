#include "physics_renderer.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PhysicsDebugRenderer::PhysicsDebugRenderer()
{
    Initialize();
}

PhysicsDebugRenderer::~PhysicsDebugRenderer() 
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PhysicsDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    Gizmos::Line(Utils::GLM_Vec3_JPH(inFrom), Utils::GLM_Vec3_JPH(inTo), Utils::GLM_Color_JPH(inColor));
}

//-----------------------------------------------------------------------------
void PhysicsDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow)
{
    Gizmos::Triangle(glm::vec3(0.0f), Utils::GLM_Vec3_JPH(inV1), Utils::GLM_Vec3_JPH(inV1), Utils::GLM_Vec3_JPH(inV1), Utils::GLM_Color_JPH(inColor));
}

//-----------------------------------------------------------------------------
void PhysicsDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight)
{
    // Implement
}
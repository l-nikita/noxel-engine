#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "utils.hpp"
#include "physics.hpp"
//#include "renderer/gizmos.hpp"

class PhysicsDebugRenderer : public JPH::DebugRendererSimple
{
public:
	PhysicsDebugRenderer()
	{
		Initialize();
	}

	void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override
	{
		//Gizmo::DrawLine(Utils::GLM_Vec3_JPH(inFrom), Utils::GLM_Vec3_JPH(inTo), Utils::GLM_Color_JPH(inColor));
	}

	void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override
	{
		//Gizmo::DrawTriangle(glm::vec3(0.0f), Utils::GLM_Vec3_JPH(inV1), Utils::GLM_Vec3_JPH(inV1), Utils::GLM_Vec3_JPH(inV1), Utils::GLM_Color_JPH(inColor));
	}

	void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight) override
	{
		// Implement
	}
};
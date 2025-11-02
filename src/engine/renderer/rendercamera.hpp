#pragma once

#include <glm/glm.hpp>
#include "common/transformable.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class RenderCamera : public Transformable
{
public:
	RenderCamera(int width, int height);
	void DebugDraw();

	void OnFrameBufferResize(int width, int height);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

private:
	int m_screenWidth;
	int m_screenHeight;
	float m_screenAspect;
};
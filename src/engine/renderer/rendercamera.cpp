#include <glm/gtc/matrix_transform.hpp>

#include "rendercamera.hpp"
#include "gizmos.hpp"
#include "dbg/debug.hpp"

constexpr auto CAMERA_FOV = 95.0f;
constexpr auto DEPTH_NEAR = 0.1f;
constexpr auto DEPTH_FAR = 100.0f;

RenderCamera::RenderCamera(int width, int height)
{
	OnFrameBufferResize(width, height);
}

void RenderCamera::DebugDraw()
{
	Gizmos::Box(m_Transform.Position, m_Transform.Rotation, glm::vec3(-0.25f, -0.25f, -0.25f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec4(1, 0, 1, 0.1f));
	Gizmos::Line(m_Transform.Position + m_Transform.Right() * 0.25f, m_Transform.Position + m_Transform.Forward() * 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::Line(m_Transform.Position + m_Transform.Left() * 0.25f, m_Transform.Position + m_Transform.Forward() * 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void RenderCamera::OnFrameBufferResize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	m_screenAspect = (float)m_screenWidth / (float)m_screenHeight;
}

glm::mat4 RenderCamera::GetViewMatrix() const
{
	return glm::lookAtLH(m_Transform.Position,
		m_Transform.Position + m_Transform.Forward(),
		m_Transform.Up());
}

glm::mat4 RenderCamera::GetProjectionMatrix() const
{
	return glm::perspectiveLH(glm::radians(CAMERA_FOV), m_screenAspect, DEPTH_NEAR, DEPTH_FAR);
}

glm::mat4 RenderCamera::GetOrthoProjectionMatrix() const
{
	float viewWidth = 30.0f;
	float viewHeight = viewWidth / m_screenAspect;

	float left = -viewWidth / 2.0f;
	float right = viewWidth / 2.0f;
	float bottom = -viewHeight / 2.0f;
	float top = viewHeight / 2.0f;

	float nearPlane = DEPTH_NEAR;
	float farPlane = DEPTH_FAR;

	return glm::orthoLH_ZO(left, right, bottom, top, nearPlane, farPlane);
}
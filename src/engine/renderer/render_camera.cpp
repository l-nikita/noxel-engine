#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include "render_camera.hpp"
#include "cmd/cmd.hpp"
#include "input/input.hpp"
#include "iengine.hpp"
#include "entities/entities.hpp"

constexpr auto CAMERA_FOV = 95.0f;
constexpr auto DEPTH_NEAR = 0.1f;
constexpr auto DEPTH_FAR = 100.0f;
constexpr auto SENSITIVITY = 0.1f;

RenderCamera::RenderCamera(int& width, int& height)
{
	OnFrameBufferResize(width, height);
}

void RenderCamera::OnFrameBufferResize(int& width, int& height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	m_screenAspect = (float)m_screenWidth / (float)m_screenHeight;
}

void RenderCamera::Update()
{
	auto delta = g_Input->GetMouseDelta();

	if (g_Input->GetRelativeMouseMode() == false)
		return;

	m_yaw -= delta.x * SENSITIVITY;
	m_pitch -= delta.y * SENSITIVITY;
	m_pitch = std::clamp(m_pitch, -85.0f, 85.0f);

	glm::quat qPitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 1, 0));
	m_Transform.Rotation = qYaw * qPitch;

	if (g_Input->IsKeyDown(SDL_SCANCODE_W))
		Move(m_Transform.Forward(), 0.01f);

	if (g_Input->IsKeyDown(SDL_SCANCODE_S))
		Move(m_Transform.Backward(), 0.01f);

	if (g_Input->IsKeyDown(SDL_SCANCODE_A))
		Move(m_Transform.Left(), 0.01f);

	if (g_Input->IsKeyDown(SDL_SCANCODE_D))
		Move(m_Transform.Right(), 0.01f);

	if (g_Input->IsKeyDown(SDL_SCANCODE_SPACE))
		Move(glm::vec3(0, 1, 0), 0.01f);

	if (g_Input->IsKeyDown(SDL_SCANCODE_LSHIFT))
		Move(glm::vec3(0, -1, 0), 0.01f);
}

void RenderCamera::Move(glm::vec3 direction, float distance)
{
	auto deltaTime = g_Engine->DeltaTime();
	m_Transform.Position += direction * distance * deltaTime;
}

glm::mat4 RenderCamera::GetViewMatrix() const
{
	return glm::lookAt(m_Transform.Position, 
		m_Transform.Position + m_Transform.Forward(), 
		m_Transform.Up());
}

glm::mat4 RenderCamera::GetProjectionMatrix() const
{
	return glm::perspective(glm::radians(CAMERA_FOV), m_screenAspect, DEPTH_NEAR, DEPTH_FAR);
}
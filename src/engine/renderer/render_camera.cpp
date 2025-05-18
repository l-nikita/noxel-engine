#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include "render_camera.hpp"
#include "cmd.hpp"
#include "input.hpp"
#include "engine.hpp"

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
	auto delta = Input::GetInstance().GetMouseDelta();

	if (Input::GetInstance().GetRelativeMouseMode() == false)
		return;

	m_Transform.Rotation.Pitch -= delta.y * SENSITIVITY;
	m_Transform.Rotation.Pitch = std::clamp(m_Transform.Rotation.Pitch, -85.0f, 85.0f);
	m_Transform.Rotation.Yaw -= delta.x * SENSITIVITY;
	m_Transform.Rotation.Normalize();

	bool isW = Input::GetInstance().IsKeyDown(SDL_SCANCODE_W);
	bool isA = Input::GetInstance().IsKeyDown(SDL_SCANCODE_A);
	bool isS = Input::GetInstance().IsKeyDown(SDL_SCANCODE_S);
	bool isD = Input::GetInstance().IsKeyDown(SDL_SCANCODE_D);
	bool isSpace = Input::GetInstance().IsKeyDown(SDL_SCANCODE_SPACE);
	bool isLShift = Input::GetInstance().IsKeyDown(SDL_SCANCODE_LSHIFT);

	if (isW)
		Move(m_Transform.Forward(), 0.005f);
	
	if (isS)
		Move(m_Transform.Backward(), 0.005f);
	
	if (isA)
		Move(m_Transform.Left(), 0.005f);
	
	if (isD)
		Move(m_Transform.Right(), 0.005f);

	if (isSpace)
		Move(glm::vec3(0.0f, 1.0f, 0.0f), 0.005f);

	if (isLShift)
		Move(glm::vec3(0.0f, -1.0f, 0.0f), 0.005f);
}

void RenderCamera::Move(glm::vec3 direction, float distance)
{
	auto deltaTime = Engine::GetInstance().DeltaTime();
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
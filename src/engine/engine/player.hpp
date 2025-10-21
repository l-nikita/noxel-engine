#pragma once

#include <algorithm>
#include "common/transformable.hpp"
#include "input/iinput.hpp"
#include "engine.hpp"

constexpr auto SENSITIVITY = 0.1f;

class Player : public Transformable
{
public:
	Player(RenderCamera* camera) 
		: m_Camera(camera)
	{
	}

	void Update()
	{
		auto delta = g_Input->GetMouseDelta();
		if (g_Input->GetRelativeMouseMode() == false)
			return;

		m_yaw += delta.x * SENSITIVITY;
		m_pitch += delta.y * SENSITIVITY;
		m_pitch = std::clamp(m_pitch, -85.0f, 85.0f);

		glm::quat qPitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));
		glm::quat qYaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 1, 0));
		m_Transform.Rotation = qYaw * qPitch;

		if (g_Input->IsKeyDown(SDL_SCANCODE_W))
			Move(m_Transform.Forward(), 10.0f);

		if (g_Input->IsKeyDown(SDL_SCANCODE_S))
			Move(m_Transform.Backward(), 10.0f);

		if (g_Input->IsKeyDown(SDL_SCANCODE_A))
			Move(m_Transform.Left(), 10.0f);

		if (g_Input->IsKeyDown(SDL_SCANCODE_D))
			Move(m_Transform.Right(), 10.0f);

		if (g_Input->IsKeyDown(SDL_SCANCODE_SPACE))
			Move(glm::vec3(0, 1, 0), 10.0f);

		if (g_Input->IsKeyDown(SDL_SCANCODE_LSHIFT))
			Move(glm::vec3(0, -1, 0), 10.0f);

		m_Camera->m_Transform = m_Transform;
	}

	void Move(glm::vec3 direction, float distance)
	{
		auto deltaTime = g_Engine->DeltaTime();
		m_Transform.Position += direction * distance * deltaTime;
	}

	RenderCamera* m_Camera = nullptr;

private:
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};

extern Player* g_Player;
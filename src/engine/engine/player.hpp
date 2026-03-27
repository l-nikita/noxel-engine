#pragma once

#include <algorithm>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include "common/transformable.hpp"
#include "input/iinput.hpp"
#include "engine.hpp"
#include "physics/physics.hpp"
#include "physics/physics_renderer.hpp"

constexpr auto SENSITIVITY = 10.0f;
constexpr auto CHARACTER_RADIUS = 0.5f;
constexpr auto CHARACTER_HEIGHT = 1.7f;
constexpr auto CHARACTER_EYE_HEIGHT = 1.65f;
constexpr auto CHARACTER_MAX_SPEED = 7.0f;

class Player
{
public:
	Player(RenderCamera* camera) 
		: m_Camera(camera)
	{
		JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
		settings->mShape = new JPH::BoxShape(JPH::Vec3(CHARACTER_RADIUS, 0.5f * CHARACTER_HEIGHT + CHARACTER_RADIUS, CHARACTER_RADIUS));

		m_character = new JPH::CharacterVirtual(settings, JPH::RVec3(0.0f, 1.0f, 0.0f), JPH::Quat::sIdentity(), GetPhysics()->GetSystem());
		m_character->SetUp(JPH::Vec3(0, 1, 0));
	}

	~Player() { }

	void UpdateCamera(float dt)
	{
		auto pos = Utils::GLM_Vec3_JPH(m_character->GetPosition()) - glm::vec3(0, (CHARACTER_HEIGHT / 2), 0);
		auto eyePos = pos + glm::vec3(0, CHARACTER_EYE_HEIGHT, 0);

		auto delta = g_Input->GetMouseDelta();
		if (g_Input->GetRelativeMouseMode())
		{
			m_yaw += delta.x * SENSITIVITY * dt;
			m_pitch += delta.y * SENSITIVITY * dt;
			m_pitch = std::clamp(m_pitch, -85.0f, 85.0f);

			glm::quat qPitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));
			glm::quat qYaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 1, 0));
			m_Camera->SetRotation(qYaw * qPitch);
		}

		if (g_Input->IsKeyPressed(SDL_SCANCODE_C))
			m_thirdPerson = !m_thirdPerson;

		auto eyeForward = m_Camera->Forward();
		if (m_thirdPerson)
			eyePos += eyeForward * -3.0f;

		m_Camera->SetPosition(eyePos);
	}

	glm::vec3 GetMovementDir()
	{
		auto dir = glm::vec3(0);

		glm::vec3 forward = glm::vec3(m_Camera->Forward().x, 0, m_Camera->Forward().z);
		glm::vec3 right = glm::vec3(m_Camera->Right().x, 0, m_Camera->Right().z);

		if (g_Input->IsKeyDown(SDL_SCANCODE_W)) dir += forward;
		if (g_Input->IsKeyDown(SDL_SCANCODE_S)) dir -= forward;
		if (g_Input->IsKeyDown(SDL_SCANCODE_A)) dir -= right;
		if (g_Input->IsKeyDown(SDL_SCANCODE_D)) dir += right;

		if (glm::length(dir) > 0.0f)
			dir = glm::normalize(dir);

		return dir;
	}

	bool IsOnGround()
	{
		return m_character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround;
	}

	float GetFriction()
	{
		if (!IsOnGround())
			return 1.2f;

		return 8.0f;
	}

	float GetWishSpeed()
	{
		return CHARACTER_MAX_SPEED;
	}

	void UpdateMovement(float dt)
	{
		m_character->UpdateGroundVelocity();

		glm::vec3 up = Utils::GLM_Vec3_JPH(m_character->GetUp());
		glm::vec3 currentVelocity = Utils::GLM_Vec3_JPH(m_character->GetLinearVelocity());
		glm::vec3 currentVerticalVelocity = Utils::GLM_Vec3_JPH(m_character->GetLinearVelocity().Dot(m_character->GetUp()) * m_character->GetUp());
		glm::vec3 groundVelocity = Utils::GLM_Vec3_JPH(m_character->GetGroundVelocity());
		glm::vec3 vel;

		m_wishVelocity = GetMovementDir();
		m_wishVelocity *= GetWishSpeed();

		if (IsOnGround())
		{
			vel = groundVelocity;

			if (g_Input->IsKeyPressed(SDL_SCANCODE_SPACE))
				vel += 8.0f * up;
		}
		else
			vel = currentVerticalVelocity;

		//JPH::RayCast ray;
		//ray.mOrigin = JPH::Vec3(10, 2, 0);
		//ray.mDirection = JPH::Vec3(0, -2, 0);

		vel += m_wishVelocity;
		vel += g_Physics->GetGravity() * dt * 2.0f;

		//std::cout << m_wishVelocity.x << " " << m_wishVelocity.y << " " << m_wishVelocity.z << std::endl;

		m_character->SetLinearVelocity(Utils::JPH_Vec3_GLM(vel));
	}

	void Update()
	{
		float dt = g_Engine->DeltaTime();

		UpdateCamera(dt);
		UpdateMovement(dt);
		UpdatePhysics(dt);
	}

	void UpdatePhysics(float dt)
	{
		JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;
		m_character->ExtendedUpdate(dt,
			-m_character->GetUp() * g_Physics->GetGravity().length(),
			updateSettings,
			GetPhysics()->GetSystem()->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
			GetPhysics()->GetSystem()->GetDefaultLayerFilter(Layers::MOVING),
			{ },
			{ },
			*GetPhysics()->GetTempAllocator());
	}

	void Draw()
	{
		if (m_thirdPerson)
		{
			JPH::RMat44 com = m_character->GetCenterOfMassTransform();
			m_character->GetShape()->Draw(JPH::DebugRenderer::sInstance, com, JPH::Vec3::sOne(), JPH::Color::sGreen, false, true);
		}
	}

	RenderCamera* m_Camera = nullptr;

private:
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	bool m_thirdPerson = false;

	JPH::Shape* m_standingShape = nullptr;
	JPH::CharacterVirtual* m_character = nullptr;

	glm::vec3 m_wishVelocity;
};

extern Player* g_Player;
#pragma once

#include <algorithm>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include "common/transformable.hpp"
#include "input/iinput.hpp"
#include "engine.hpp"
#include "physics/physics.hpp"

constexpr auto SENSITIVITY = 10.0f;
constexpr auto CHARACTER_RADIUS = 0.3f;
constexpr auto CHARACTER_HEIGHT = 1.7f;
constexpr auto ENABLE_CHARACTER_INERTIA = true;

class Player
{
public:
	Player(RenderCamera* camera) 
		: m_Camera(camera)
	{
		JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
		settings->mShape = new JPH::BoxShape(JPH::Vec3(CHARACTER_RADIUS, 0.5f * CHARACTER_HEIGHT + CHARACTER_RADIUS, CHARACTER_RADIUS));

		m_Character = new JPH::CharacterVirtual(settings, JPH::RVec3(0.0f, 1.0f, 0.0f), JPH::Quat::sIdentity(), GetPhysics()->GetSystem());
		m_Character->SetUp(JPH::Vec3(0, 1, 0));
	}

	~Player() { }

	void UpdateCamera(float dt)
	{
		auto eyePos = Utils::GLM_Vec3_JPH(m_Character->GetPosition());

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

		auto eyeForward = m_Camera->Forward();
		m_Camera->SetPosition(eyePos + eyeForward * -3.0f);
	}

	void UpdateDesiredVelocity(float dt)
	{
		m_DesiredVelocity = glm::vec3(0);

		glm::vec3 forward = glm::vec3(m_Camera->Forward().x, 0, m_Camera->Forward().z);
		glm::vec3 right = glm::vec3(m_Camera->Right().x, 0, m_Camera->Right().z);

		if (g_Input->IsKeyDown(SDL_SCANCODE_W)) m_DesiredVelocity += forward;
		if (g_Input->IsKeyDown(SDL_SCANCODE_S)) m_DesiredVelocity -= forward;
		if (g_Input->IsKeyDown(SDL_SCANCODE_A)) m_DesiredVelocity -= right;
		if (g_Input->IsKeyDown(SDL_SCANCODE_D)) m_DesiredVelocity += right;

		if (glm::length(m_DesiredVelocity) > 0.0f)
			m_DesiredVelocity = glm::normalize(m_DesiredVelocity) * 5.0f;
	}

	void Update()
	{
		float dt = g_Engine->DeltaTime();

		UpdateCamera(dt);
		UpdateDesiredVelocity(dt);

		m_Character->UpdateGroundVelocity();

		glm::vec3 currentVerticalVelocity = Utils::GLM_Vec3_JPH(m_Character->GetLinearVelocity().Dot(m_Character->GetUp()) * m_Character->GetUp());
		glm::vec3 groundVelocity = Utils::GLM_Vec3_JPH(m_Character->GetGroundVelocity());
		glm::vec3 newVelocity;

		bool movingTowardsGround = (currentVerticalVelocity.y - groundVelocity.y) < 0.1f;
		if (m_Character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround	// If on ground
			&& (ENABLE_CHARACTER_INERTIA ?
				movingTowardsGround													// Inertia enabled: And not moving away from ground
				: !m_Character->IsSlopeTooSteep(m_Character->GetGroundNormal())))			// Inertia disabled: And not on a slope that is too steep
		{
			newVelocity = groundVelocity;

			if (g_Input->IsKeyDown(SDL_SCANCODE_SPACE) && movingTowardsGround)
				newVelocity += 5.0f * Utils::GLM_Vec3_JPH(m_Character->GetUp());
		}
		else
			newVelocity = currentVerticalVelocity;

		newVelocity += m_DesiredVelocity;
		newVelocity += g_Physics->GetGravity() * dt;

		m_Character->SetLinearVelocity(Utils::JPH_Vec3_GLM(newVelocity));

		UpdatePhysics(dt);
	}

	void UpdatePhysics(float dt)
	{
		JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;
		m_Character->ExtendedUpdate(dt,
			-m_Character->GetUp() * g_Physics->GetGravity().length(),
			updateSettings,
			GetPhysics()->GetSystem()->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
			GetPhysics()->GetSystem()->GetDefaultLayerFilter(Layers::MOVING),
			{ },
			{ },
			*GetPhysics()->GetTempAllocator());
	}

	void Draw()
	{
		JPH::RMat44 com = m_Character->GetCenterOfMassTransform();
		m_Character->GetShape()->Draw(JPH::DebugRenderer::sInstance, com, JPH::Vec3::sOne(), JPH::Color::sGreen, false, true);
	}

	RenderCamera* m_Camera = nullptr;

	JPH::Shape* m_StandingShape = nullptr;
	JPH::CharacterVirtual* m_Character = nullptr;

	glm::vec3 m_DesiredVelocity;

private:
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};

extern Player* g_Player;
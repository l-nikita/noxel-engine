#pragma once

#include <glm/glm.hpp>
#include "common/transform.hpp"

class RenderCamera
{
public:
	Transform m_Transform;
	RenderCamera(int width, int height);

	void OnFrameBufferResize(int width, int height);
	void Update();
	void Move(glm::vec3 direction, float distance);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

private:
	int m_screenWidth;
	int m_screenHeight;
	float m_screenAspect;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};
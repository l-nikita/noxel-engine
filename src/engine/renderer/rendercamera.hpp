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

	float GetFOV() const { return m_fov; }
	void SetFOV(float _fov) { m_fov = _fov; }

	float GetNear() const { return m_near; }
	void SetNear(float _near) { m_near = _near; }

	float GetFar() const { return m_far; }
	void SetFar(float _far) { m_far = _far; }

	float GetAspect() const { return m_aspect; }

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetOrthoProjectionMatrix() const;

private:
	int m_width;
	int m_height;
	float m_aspect;
	float m_fov;
	float m_near;
	float m_far;
};
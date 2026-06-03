#include <glm/gtc/matrix_transform.hpp>

#include "rendercamera.hpp"
#include "gizmos.hpp"
#include "dbg/debug.hpp"

constexpr auto DEPTH_NEAR = 0.1f;
constexpr auto DEPTH_FAR = 100.0f;

RenderCamera::RenderCamera( const int width, const int height )
{
    OnFrameBufferResize(width, height);
}

void RenderCamera::DebugDraw() const
{
    auto& trans = GetTransform();
    Gizmos::Box(trans.Position, trans.Rotation, glm::vec3(-0.25f, -0.25f, -0.25f), glm::vec3(0.25f, 0.25f, 0.25f),
                glm::vec4(1, 0, 1, 0.1f));
    Gizmos::Line(trans.Position + trans.Right() * 0.25f, trans.Position + trans.Forward() * 1.0f,
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Gizmos::Line(trans.Position + trans.Left() * 0.25f, trans.Position + trans.Forward() * 1.0f,
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void RenderCamera::OnFrameBufferResize( int width, int height )
{
    m_width = width;
    m_height = height;
    m_aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
}

glm::mat4 RenderCamera::GetViewMatrix() const
{
    auto& trans = GetTransform();
    return glm::lookAtLH(trans.Position,
                         trans.Position + trans.Forward(),
                         trans.Up());
}

glm::mat4 RenderCamera::GetProjectionMatrix() const
{
    return glm::perspectiveLH_ZO(glm::radians(m_fov), m_aspect, m_near, m_far);
}

glm::mat4 RenderCamera::GetOrthoProjectionMatrix() const
{
    constexpr auto viewWidth = 30.0f;
    const float viewHeight = viewWidth / m_aspect;

    constexpr float left = -viewWidth / 2.0f;
    float right = viewWidth / 2.0f;
    float bottom = -viewHeight / 2.0f;
    float top = viewHeight / 2.0f;

    float nearPlane = DEPTH_NEAR;
    float farPlane = DEPTH_FAR;

    return glm::orthoLH_ZO(left, right, bottom, top, nearPlane, farPlane);
}

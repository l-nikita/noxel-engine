#pragma once

#include <glm/glm.hpp>

#include "common/transformable.hpp"

namespace render
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Camera : public Transformable
    {
    public:
        Camera( int width, int height );
        void DebugDraw() const;

        void OnFrameBufferResize( int width, int height );

        float GetFOV() const
        {
            return m_fov;
        }

        void SetFOV( float _fov )
        {
            m_fov = _fov;
        }

        float GetNear() const
        {
            return m_near;
        }

        void SetNear( float _near )
        {
            m_near = _near;
        }

        float GetFar() const
        {
            return m_far;
        }

        void SetFar( float _far )
        {
            m_far = _far;
        }

        float GetAspect() const
        {
            return m_aspect;
        }

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::mat4 GetOrthoProjectionMatrix() const;

    private:
        int m_width = 0;
        int m_height = 0;
        float m_aspect = 0.0f;
        float m_fov = 0.0f;
        float m_near = 0.0f;
        float m_far = 0.0f;
    };
}

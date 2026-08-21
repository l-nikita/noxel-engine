#pragma once

#include "common/transformable.hpp"
#include "renderer/lighting/lightcommon.hpp"

namespace render::lighting
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Light final : public Transformable<Light>
    {
    public:
        explicit Light( LightType type, bool shadowsEnabled = false )
            : m_type( type ), m_shadowsEnabled( shadowsEnabled )
        {
            SetOuterCutoff( m_outerCutoffAngle );
            SetInnerCutoff( m_innerCutoffAngle );
        }

        bool IsA( const LightType type ) const { return m_type == type; }
        LightType GetType() const { return m_type; }

        bool IsShadowsEnabled() const { return m_shadowsEnabled; }
        float GetShadowSoftness() const { return m_shadowSoftness; }

        bool IsEnabled() const { return m_enabled; }

        float GetRadius() const { return m_radius; }

        float GetOuterCutoff() const { return m_outerCutoff; }
        float GetOuterCutoffAngle() const { return m_outerCutoffAngle; }

        float GetInnerCutoff() const { return m_innerCutoff; }
        float GetInnerCutoffAngle() const { return m_innerCutoffAngle; }

        const glm::vec3& GetColor() const { return m_color; }
        float GetBrightness() const { return m_brightness; }

        Light& SetEnabled( bool value )
        {
            m_enabled = value;
            return *this;
        }

        Light& Toggle()
        {
            m_enabled = !m_enabled;
            return *this;
        }

        Light& SetRadius( float radius )
        {
            m_radius = glm::max( radius, 0.0f );
            return *this;
        }

        Light& SetShadowSoftness( float softness )
        {
            m_shadowSoftness = glm::max( softness, 0.0f );
            return *this;
        }

        Light& SetOuterCutoff( float angle )
        {
            m_outerCutoffAngle = glm::clamp( angle, 0.0f, 180.0f );
            m_innerCutoffAngle = glm::min( m_innerCutoffAngle, m_outerCutoffAngle );

            m_outerCutoff = std::cos( glm::radians( m_outerCutoffAngle * 0.5f ) );
            m_innerCutoff = std::cos( glm::radians( m_innerCutoffAngle * 0.5f ) );

            return *this;
        }

        Light& SetInnerCutoff( float angle )
        {
            m_innerCutoffAngle = glm::clamp( angle, 0.0f, 180.0f );
            m_outerCutoffAngle = glm::max( m_outerCutoffAngle, m_innerCutoffAngle );

            m_innerCutoff = std::cos( glm::radians( m_innerCutoffAngle * 0.5f ) );
            m_outerCutoff = std::cos( glm::radians( m_outerCutoffAngle * 0.5f ) );

            return *this;
        }

        Light& SetColor( const glm::vec3& color )
        {
            m_color = glm::clamp( color, 0.0f, 1.0f );
            return *this;
        }

        Light& SetBrightness( float brightness )
        {
            m_brightness = glm::max( brightness, 0.0f );
            return *this;
        }

        glm::mat4 GetViewMatrix() const
        {
            auto& trans = GetTransform();
            return glm::lookAtLH( trans.Position,
                                  trans.Position + trans.Forward(),
                                  trans.Up() );
        }

        glm::mat4 GetProjectionMatrix() const
        {
            if ( IsA( LIGHT_DIRECTIONAL ) )
            {
                const float halfExtent = m_radius * 0.5f;
                return glm::orthoLH_ZO(
                    -halfExtent,
                    halfExtent,
                    -halfExtent,
                    halfExtent,
                    0.1f,
                    m_radius * 2.0f
                );
            }

            return glm::perspectiveLH_ZO( glm::radians( m_outerCutoffAngle ), 1.0f, 0.1f, GetRadius() );
        }

    private:
        LightType m_type;

        bool m_enabled = true;
        bool m_shadowsEnabled = true;

        float m_radius = 15.0f;
        float m_shadowSoftness = 0.01f;

        float m_outerCutoffAngle = 75.0f;
        float m_outerCutoff = 0.0f;

        float m_innerCutoffAngle = 65.0f;
        float m_innerCutoff = 0.0f;

        glm::vec3 m_color = glm::vec3( 1 );
        float m_brightness = 1.0f;
    };
}

#pragma once

#include <glm/glm.hpp>

#include "../common/utils.hpp"
#include "common/singleton.hpp"
#include "common/transformable.hpp"

namespace render
{
    constexpr auto MAX_LIGHTS = 6;
    constexpr auto MAX_SHADOWS_MAPS = 4;
    constexpr auto SHADOW_MAP_SIZE = 2048;

    enum LightType
    {
        LIGHT_POINT,
        LIGHT_SPOT,
        LIGHT_DIRECTIONAL,
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class ShadowMap
    {
    public:
        explicit ShadowMap( std::uint32_t shadowMapSize = SHADOW_MAP_SIZE )
            : m_shadowMapSize( shadowMapSize )
        {
        }

        ~ShadowMap()
        {
            Release();
        }

        void Release()
        {
        }

        const std::uint32_t& GetSize() const
        {
            return m_shadowMapSize;
        }

    private:
        std::uint32_t m_shadowMapSize = SHADOW_MAP_SIZE;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Light : public Transformable<Light>
    {
    public:
        explicit Light( LightType type, bool shadowsEnabled = false, std::uint32_t shadowMapSize = SHADOW_MAP_SIZE )
            : m_type( type )
        {
            if ( shadowsEnabled && ( IsA( LIGHT_SPOT ) || IsA( LIGHT_DIRECTIONAL ) ) )
                m_shadowMap = std::make_unique<ShadowMap>( shadowMapSize );
        }

        bool IsA( LightType type ) const { return m_type == type; }
        LightType GetType() const { return m_type; }

        bool IsShadowsEnabled() const { return m_shadowMap != nullptr; }
        const ShadowMap* GetShadowMap() const { return m_shadowMap.get(); }
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

            return glm::perspectiveLH_ZO( glm::radians( m_outerCutoff ), 1.0f, 0.1f, GetRadius() );
        }

    private:
        LightType m_type;

        std::unique_ptr<ShadowMap> m_shadowMap;

        bool m_enabled = true;

        float m_radius = 15.0f;
        float m_shadowSoftness = 0.01f;

        float m_outerCutoffAngle = 75.0f;
        float m_outerCutoff = 0.0f;

        float m_innerCutoffAngle = 65.0f;
        float m_innerCutoff = 0.0f;

        glm::vec3 m_color = glm::vec3( 1 );
        float m_brightness = 1.0f;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    using Lights = std::vector<std::unique_ptr<Light>>;

    class LightManager : public Singleton<LightManager>
    {
    public:
        LightManager()
        {
            m_lights.reserve( MAX_LIGHTS );
        }

        ~LightManager() override
        {
            m_lights.clear();
        }

        template<typename... Args>
        Light* CreateLight( Args&&... args )
        {
            auto light = std::make_unique<Light>( std::forward<Args>( args )... );
            auto* ptr = light.get();

            m_lights.push_back( std::move( light ) );

            return ptr;
        }

        void RemoveLight( Light* light )
        {
            if ( !light )
                return;

            std::erase_if( m_lights, [light]( const std::unique_ptr<Light>& ptr )
            {
                return ptr.get() == light;
            } );
        }

        const Lights& GetLights() const
        {
            return m_lights;
        }

    private:
        Lights m_lights = {};
    };
}

#pragma once

#include <array>
#include "glm/glm.hpp"
#include "idepthstencil.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "common/singleton.hpp"
#include "common/transformable.hpp"

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
    ShadowMap( std::uint32_t shadowMapSize = SHADOW_MAP_SIZE )
        : m_shadowMapSize( shadowMapSize )
    {
        auto factory = g_Renderer->GetFactory();
        m_depth = factory->CreateDepthStencil( GetSize(), GetSize(), true );
    }

    ~ShadowMap()
    {
        Release();
    }

    void Release()
    {
        auto factory = g_Renderer->GetFactory();
        if ( m_depth )
        {
            factory->Release( m_depth );
            m_depth = nullptr;
        }
    }

    IDepthStencil* GetDepth()
    {
        return m_depth;
    }

    int GetSize()
    {
        return m_shadowMapSize;
    }

private:
    std::uint32_t m_shadowMapSize = SHADOW_MAP_SIZE;
    IDepthStencil* m_depth = nullptr;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Light : public Transformable
{
public:
    explicit Light( LightType type, bool shadowsEnabled = false, std::uint32_t shadowMapSize = SHADOW_MAP_SIZE )
        : m_type( type )
    {
        if ( shadowsEnabled && ( IsA( LIGHT_SPOT ) || IsA( LIGHT_DIRECTIONAL ) ) )
            m_shadowMap = new ShadowMap( shadowMapSize );
    }

    ~Light()
    {
        Release();
    }

    void Release()
    {
        if ( m_shadowMap )
            delete m_shadowMap, m_shadowMap = nullptr;
    }

    bool IsA( LightType type ) const
    {
        return m_type == type;
    }

    LightType GetType() const
    {
        return m_type;
    }

    bool IsShadowsEnabled() const
    {
        return m_shadowMap != nullptr;
    }

    ShadowMap* GetShadowMap() const
    {
        return m_shadowMap;
    }

    bool IsEnabled() const
    {
        return m_enabled;
    }

    float GetRadius() const
    {
        return m_radius;
    }

    float GetFOV() const
    {
        return m_fov;
    }

    float GetShadowSoftness() const
    {
        return m_shadowSoftness;
    }

    glm::vec3 GetColor() const
    {
        return m_color;
    }

    void SetEnabled( const bool value )
    {
        m_enabled = value;
    }

    void SetRadius( const float radius )
    {
        m_radius = radius;
    }

    void SetFOV( const float fov )
    {
        m_fov = fov;
    }

    void SetShadowSoftness( const float softness )
    {
        m_shadowSoftness = glm::max( softness, 0.0f );
    }

    void SetColor( glm::vec3 color )
    {
        m_color = color;
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

        return glm::perspectiveLH_ZO( glm::radians( m_fov ), 1.0f, 0.1f, GetRadius() );
    }

private:
    LightType m_type;

    ShadowMap* m_shadowMap = nullptr;

    bool m_enabled = true;
    float m_radius = 15;
    float m_fov = 75;
    float m_shadowSoftness = 0.01f;
    glm::vec3 m_color = glm::vec3( 1 );
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using Lights = std::array<Light*, MAX_LIGHTS>;

class LightManager : public Singleton<LightManager>
{
public:
    ~LightManager()
    {
        Release();
    }

    void Release()
    {
        for ( int i = 0; i < MAX_LIGHTS; ++i )
        {
            if ( m_lights[ i ] )
                delete m_lights[ i ];

            m_lights[ i ] = nullptr;
        }
    }

    void AddLight( Light* light )
    {
        for ( auto& slot: m_lights )
        {
            if ( slot == nullptr )
            {
                slot = light;
                return;
            }
        }
    }

    void RemoveLight( const Light* light )
    {
        for ( auto& slot: m_lights )
        {
            if ( slot == light )
            {
                delete slot;
                slot = nullptr;
                return;
            }
        }
    }

    const Lights& GetLights() const
    {
        return m_lights;
    }

private:
    Lights m_lights = {};
};

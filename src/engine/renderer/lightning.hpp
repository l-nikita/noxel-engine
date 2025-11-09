#pragma once

#include <array>
#include "glm/glm.hpp"
#include "common/transformable.hpp"
#include "common/singleton.hpp"
#include "rendercamera.hpp"
#include "idepthstencil.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"

constexpr auto MAX_LIGHTS = 6;
constexpr auto MAX_SHADOWS_MAPS = 4;
constexpr auto SHADOW_MAP_SIZE = 1024;

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
    ShadowMap()
    {
        auto factory = g_Renderer->GetFactory();
        m_depth = factory->CreateDepthStencil(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, true);
    }

    ~ShadowMap() { Release(); }

    void Release()
    {
        auto factory = g_Renderer->GetFactory();
        if (m_depth)
        {
            factory->Release(m_depth);
            m_depth = nullptr;
        }
    }

    IDepthStencil* GetDepth() { return m_depth; }
    int GetSize() const { return SHADOW_MAP_SIZE; }

private:
    IDepthStencil* m_depth = nullptr;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Light : public Transformable
{
public:
    Light(LightType type, bool shadowsEnabled = false)
        : m_type(type)
    { 
        m_camera = new RenderCamera(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        m_camera->SetNear(0.1f);

        if (shadowsEnabled && IsA(LIGHT_SPOT))
            m_shadowMap = new ShadowMap();
    }

    ~Light() { Release(); }

    void Release()
    {
        if (m_camera)
        {
            delete m_camera;
            m_camera = nullptr;
        }

        if (m_shadowMap)
        {
            delete m_shadowMap;
            m_shadowMap = nullptr;
        }
    }

    bool IsA(LightType type) const { return m_type == type; }
    LightType GetType() const { return m_type; }

    bool IsShadowsEnabled() const { return m_shadowMap != nullptr; }
    ShadowMap* GetShadowMap() const { return m_shadowMap; }

    float GetRadius() const { return m_radius; }
    float GetFOV() const { return m_fov; }
    glm::vec3 GetColor() const { return m_color; }

    void SetRadius(float radius)
    {
        m_radius = radius;
        m_camera->SetFar(radius);
    }

    void SetFOV(float fov)
    {
        m_fov = fov;
        m_camera->SetFOV(fov);
    }

    void SetColor(glm::vec3 color) 
    { 
        m_color = color; 
    }

    glm::mat4 GetViewMatrix() const { return m_camera->GetViewMatrix(); }
    glm::mat4 GetProjectionMatrix() const { return m_camera->GetProjectionMatrix(); }

    RenderCamera* GetCamera() const { return m_camera; }

private:
    LightType m_type;

    RenderCamera* m_camera = nullptr;
    ShadowMap* m_shadowMap = nullptr;

    float m_radius = 15;
    float m_fov = 75;
    glm::vec3 m_color = glm::vec3(1);
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
        for (int i = 0; i < MAX_LIGHTS; ++i)
        {
            if (m_lights[i])
                delete m_lights[i];

            m_lights[i] = nullptr;
        }
    }

    void AddLight(Light* light)
    {
        for (auto& slot : m_lights)
        {
            if (slot == nullptr)
            {
                slot = light;
                return;
            }
        }
    }

    void RemoveLight(Light* light)
    {
        for (auto& slot : m_lights)
        {
            if (slot == light)
            {
                delete slot;
                slot = nullptr;
                return;
            }
        }
    }

    const Lights& GetLights() const { return m_lights; }

private:
    Lights m_lights;
};
#pragma once

#include "glm/glm.hpp"
#include <string>

struct CBTransform
{
    glm::mat4 Model;
    glm::mat4 View;
    glm::mat4 Projection;
};

struct CBColor
{
    glm::vec4 Color;
};

struct CBLight
{
    int Type;
    int pad0, pad1, pad2;
    glm::vec3 Pos;
    float Radius;
    glm::vec3 Color;
    float Cutoff;
    glm::vec3 Dir;
    int pad3;
};

struct CBLightsData
{
    CBLight Lights[4];
    glm::vec3 ViewPos;
    float pad0;
};

struct CBLightMatrices
{
    glm::mat4 LightView;
    glm::mat4 LightProjection;
};

constexpr auto MAX_LIGHTS = 6;

enum LightType
{
    LIGHT_POINT,
    LIGHT_SPOT,
    LIGHT_DIRECTIONAL,
};

class Light : public Transformable
{
public:
    Light(LightType type) 
        : m_type(type)
    { 
        m_camera = new RenderCamera(512, 512);
        m_camera->SetNear(0.1f);
    }

    ~Light() { delete m_camera; }

    bool IsA(LightType type) const { return m_type == type; }
    LightType GetType() const { return m_type; }

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

    void SetColor(glm::vec3 color) { m_color = color; }

    RenderCamera* m_camera = nullptr;

    glm::mat4 GetViewMatrix() const
    {
	    return m_camera->GetViewMatrix();
    }

    glm::mat4 GetProjectionMatrix() const
    {
        return m_camera->GetProjectionMatrix();
    }

private:
    LightType m_type;

    float m_radius;
    float m_fov;
    glm::vec3 m_color;
};

using Lights = std::vector<Light*>;

class LightManager : public Singleton<LightManager>
{
public:
    ~LightManager()
    {
        for (auto light : m_lights)
            delete light;

        m_lights.clear();
    }

    void AddLight(Light* light)
    {
        m_lights.push_back(light);
    }

    void RemoveLight(Light* light)
    {
        auto rem = std::remove(m_lights.begin(), m_lights.end(), light);
        m_lights.erase(rem, m_lights.end());
    }

    Lights GetLights() const { return m_lights; }

private:
    Lights m_lights;
};
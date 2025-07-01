#pragma once

#include <vector>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "iengine.hpp"
#include "common/singleton.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "render_camera.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Gizmos : public Singleton<Gizmos>
{
public:
    std::shared_ptr<Material> m_Material;

    std::shared_ptr<Mesh> m_BoxMesh;
    std::shared_ptr<Mesh> m_LineMesh;
    std::shared_ptr<Mesh> m_TriangleMesh;

    bool Init()
    {
        m_Material = Material::Get("gizmos");

        InitBox();
        InitLine();
        InitTriangle();

        return true;
    }

    void InitBox()
    {
        glm::vec3 min(-1.0f);
        glm::vec3 max(1.0f);

        std::vector<glm::vec3> corners = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {max.x, max.y, min.z}, {min.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {max.x, max.y, max.z}, {min.x, max.y, max.z}
        };

        // Indices for 12 edges of a box (wireframe)
        std::vector<int> indices = {
            0, 1, 1, 2, 2, 3, 3, 0, // bottom face
            4, 5, 5, 6, 6, 7, 7, 4, // top face
            0, 4, 1, 5, 2, 6, 3, 7  // vertical edges
        };

        std::vector<float> vertices;
        for (const auto& v : corners)
        {
            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
        }

        MeshData data(vertices, indices, {}, GL_LINES);
        m_BoxMesh = std::make_shared<Mesh>(data);
    }

    void InitLine()
    {
        std::vector<int> indices = {
            0, 1
        };

        std::vector<float> vertices = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f
        };

        MeshData data(vertices, indices, {}, GL_LINES);
        m_LineMesh = std::make_shared<Mesh>(data);
    }

    void InitTriangle()
    {
        std::vector<int> indices = {
            0, 1, 2
        };

        std::vector<float> vertices = {
            0.0f, 0.0f, 0.0f, // p0
            1.0f, 0.0f, 0.0f, // p1
            0.0f, 1.0f, 0.0f  // p2
        };

        MeshData data(vertices, indices, {}, GL_TRIANGLES);
        m_TriangleMesh = std::make_shared<Mesh>(data);
    }
};

namespace Gizmo
{
    static void DrawBox(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& min, const glm::vec3& max, const glm::vec4& color = glm::vec4(1.0f))
    {
        glm::vec3 scale = (max - min) * 0.5f;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) 
            * glm::toMat4(rotation)
            * glm::scale(glm::mat4(1.0f), scale);

        auto& gizmos = Gizmos::GetInstance();
        auto& shader = gizmos.m_Material->GetShader();

        shader.Bind();
        shader.SetVec4("color", color);
        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", g_Renderer->GetCamera()->GetViewMatrix());
        shader.SetMatrix4("projection", g_Renderer->GetCamera()->GetProjectionMatrix());

        gizmos.m_BoxMesh->Draw();
        shader.Unbind();
    }

    static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f))
    {
        glm::vec3 dir = end - start;
        float length = glm::length(dir);
        if (length <= 0.0001f)
            return;

        glm::vec3 direction = glm::normalize(dir);
        glm::quat rotation = glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), direction);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), start) *
            glm::mat4_cast(rotation) *
            glm::scale(glm::mat4(1.0f), glm::vec3(length, 1.0f, 1.0f));

        auto& gizmos = Gizmos::GetInstance();
        auto& shader = gizmos.m_Material->GetShader();
        shader.Bind();
        shader.SetVec4("color", color);
        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", g_Renderer->GetCamera()->GetViewMatrix());
        shader.SetMatrix4("projection", g_Renderer->GetCamera()->GetProjectionMatrix());

        gizmos.m_LineMesh->Draw();
        shader.Unbind();
    }

    static void DrawTriangle(
        const glm::vec3& position,
        const glm::vec3& v1, 
        const glm::vec3& v2, 
        const glm::vec3& v3, 
        const glm::vec4& color = glm::vec4(1.0f))
    {
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        glm::mat3 basis(edge1, edge2, normal);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, v1);
        model *= glm::mat4(glm::mat3(basis)); // scale/rotate
        model *= glm::translate(glm::mat4(1.0f), position);

        auto& gizmos = Gizmos::GetInstance();
        auto& shader = gizmos.m_Material->GetShader();
        shader.Bind();
        shader.SetVec4("color", color);
        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", g_Renderer->GetCamera()->GetViewMatrix());
        shader.SetMatrix4("projection", g_Renderer->GetCamera()->GetProjectionMatrix());

        gizmos.m_TriangleMesh->Draw();
        shader.Unbind();
    }

    static void DrawWorldCoords(const glm::vec3& position, const glm::vec4& color = glm::vec4(1.0f))
    {
        DrawLine(position, position + glm::vec3(1, 0, 0), glm::vec4(1, 0, 0, 1)); // Red
        DrawLine(position, position + glm::vec3(0, 1, 0), glm::vec4(0, 1, 0, 1)); // Green
        DrawLine(position, position + glm::vec3(0, 0, 1), glm::vec4(0, 0, 1, 1)); // Blue
    }

    static void DrawGrid(const glm::vec3& position, const glm::quat& quat, int slices = 20, float gridSize = 20.0f, const glm::vec4& color = glm::vec4(0.5f))
    {
        static std::shared_ptr<Mesh> cachedMesh;
        static int cachedSlices = -1;
        static float cachedGridSize = -1.0f;

        if (!cachedMesh || cachedSlices != slices || cachedGridSize != gridSize)
        {
            float half = gridSize * 0.5f;
            float step = gridSize / static_cast<float>(slices);

            std::vector<float> vertices;
            std::vector<int> indices;

            int index = 0;
            for (int i = 0; i <= slices; ++i)
            {
                float pos = -half + i * step;

                vertices.push_back(-half); vertices.push_back(0.0f); vertices.push_back(pos);
                vertices.push_back(half);  vertices.push_back(0.0f); vertices.push_back(pos);
                indices.push_back(index++);
                indices.push_back(index++);

                vertices.push_back(pos); vertices.push_back(0.0f); vertices.push_back(-half);
                vertices.push_back(pos); vertices.push_back(0.0f); vertices.push_back(half);
                indices.push_back(index++);
                indices.push_back(index++);
            }

            MeshData data(vertices, indices, {}, GL_LINES);
            cachedMesh = std::make_shared<Mesh>(data);
            cachedSlices = slices;
            cachedGridSize = gridSize;
        }

        auto& gizmos = Gizmos::GetInstance();
        auto& shader = gizmos.m_Material->GetShader();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position)
            * glm::toMat4(quat);

        shader.Bind();
        shader.SetVec4("color", color);
        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", g_Renderer->GetCamera()->GetViewMatrix());
        shader.SetMatrix4("projection", g_Renderer->GetCamera()->GetProjectionMatrix());

        cachedMesh->Draw();
        shader.Unbind();
    }

    static void DrawSphere(const glm::vec3& position, const glm::quat& quat, float radius = 1.0f, int rings = 16, int segments = 32, const glm::vec4& color = glm::vec4(1.0f))
    {
        static std::shared_ptr<Mesh> sphereMesh;
        static int cachedRings = -1;
        static int cachedSegments = -1;

        if (!sphereMesh || cachedRings != rings || cachedSegments != segments)
        {
            std::vector<float> vertices;
            std::vector<int> indices;

            // Generate wireframe sphere (longitude + latitude)
            for (int i = 0; i <= rings; ++i)
            {
                float theta = glm::pi<float>() * float(i) / float(rings);
                float sinTheta = sin(theta);
                float cosTheta = cos(theta);

                for (int j = 0; j <= segments; ++j)
                {
                    float phi = 2.0f * glm::pi<float>() * float(j) / float(segments);
                    float sinPhi = sin(phi);
                    float cosPhi = cos(phi);

                    float x = cosPhi * sinTheta;
                    float y = cosTheta;
                    float z = sinPhi * sinTheta;

                    vertices.push_back(x);
                    vertices.push_back(y);
                    vertices.push_back(z);
                }
            }

            // Indices for wireframe (latitudes and longitudes)
            for (int i = 0; i < rings; ++i)
            {
                for (int j = 0; j < segments; ++j)
                {
                    int first = i * (segments + 1) + j;
                    int second = first + segments + 1;

                    // Horizontal ring
                    indices.push_back(first);
                    indices.push_back(first + 1);

                    // Vertical line
                    indices.push_back(first);
                    indices.push_back(second);
                }
            }

            MeshData data(vertices, indices, {}, GL_LINES);
            sphereMesh = std::make_shared<Mesh>(data);
            cachedRings = rings;
            cachedSegments = segments;
        }

        auto& gizmos = Gizmos::GetInstance();
        auto& shader = gizmos.m_Material->GetShader();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position)
            * glm::toMat4(quat)
            * glm::scale(glm::mat4(1.0f), glm::vec3(radius));

        shader.Bind();
        shader.SetVec4("color", color);
        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", g_Renderer->GetCamera()->GetViewMatrix());
        shader.SetMatrix4("projection", g_Renderer->GetCamera()->GetProjectionMatrix());

        sphereMesh->Draw();
        shader.Unbind();
    }
}
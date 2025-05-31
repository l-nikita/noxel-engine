#pragma once

#include <vector>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "singleton.hpp"
#include "material.hpp"
#include "mesh.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Gizmos : public Singleton<Gizmos>
{
public:
    std::shared_ptr<Material> m_Material;
    std::shared_ptr<Mesh> m_BoxMesh;
    std::shared_ptr<Mesh> m_LineMesh;

    bool Init()
    {
        m_Material = Material::Get("gizmos");

        InitBox();
        InitLine();

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
        std::vector<int> indices =
        {
            0, 1
        };

        std::vector<float> vertices =
        {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f
        };

        MeshData data(vertices, indices, {}, GL_LINES);
        m_LineMesh = std::make_shared<Mesh>(data);
    }
};

namespace Gizmo
{
    static void DrawBox(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& min, const glm::vec3& max, const glm::vec3& color = glm::vec3(1.0f))
    {
        glm::vec3 scale = (max - min) * 0.5f;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) 
            * glm::toMat4(rotation)
            * glm::scale(glm::mat4(1.0f), scale);

        auto& gizmos = Gizmos::GetInstance();
        auto& shader = gizmos.m_Material->GetShader();

        shader.Bind();
        shader.SetVec3("color", color);
        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", Renderer::GetInstance().m_Camera->GetViewMatrix());
        shader.SetMatrix4("projection", Renderer::GetInstance().m_Camera->GetProjectionMatrix());

        gizmos.m_BoxMesh->Draw();
        shader.Unbind();
    }

    static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f))
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
        shader.SetVec3("color", color);
        shader.SetMatrix4("model", model);
        shader.SetMatrix4("view", Renderer::GetInstance().m_Camera->GetViewMatrix());
        shader.SetMatrix4("projection", Renderer::GetInstance().m_Camera->GetProjectionMatrix());

        gizmos.m_LineMesh->Draw();
        shader.Unbind();
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    //class Grid : public BaseGizmo
    //{
    //protected:
    //    std::unique_ptr<Model> m_model;

    //    Grid(const int& slices, const float& gridSize, const glm::vec3& color)
    //    {
    //        int vertexCount = (slices + 1) * (slices + 1);
    //        std::vector<float> vertices(vertexCount * 3);

    //        int indexCount = slices * slices * 8;
    //        std::vector<int> indices(indexCount);

    //        int vi = 0;
    //        for (int j = 0; j <= slices; ++j)
    //        {
    //            for (int i = 0; i <= slices; ++i)
    //            {
    //                float x = static_cast<float>(i) / slices * gridSize;
    //                float y = 0.0f;
    //                float z = static_cast<float>(j) / slices * gridSize;

    //                vertices[vi++] = x;
    //                vertices[vi++] = y;
    //                vertices[vi++] = z;
    //            }
    //        }

    //        int ii = 0;
    //        for (int j = 0; j < slices; ++j)
    //        {
    //            for (int i = 0; i < slices; ++i)
    //            {
    //                int row1 = j * (slices + 1);
    //                int row2 = (j + 1) * (slices + 1);

    //                indices[ii++] = row1 + i;
    //                indices[ii++] = row1 + i + 1;

    //                indices[ii++] = row1 + i + 1;
    //                indices[ii++] = row2 + i + 1;

    //                indices[ii++] = row2 + i + 1;
    //                indices[ii++] = row2 + i;

    //                indices[ii++] = row2 + i;
    //                indices[ii++] = row1 + i;
    //            }
    //        }

    //        MeshData data(vertices, indices, {}, GL_LINES);
    //        auto material = Material::Get("gizmos");
    //        auto& shader = material->GetShader();
    //        shader.Bind();
    //        shader.SetVec3("color", color);
    //        shader.Unbind();

    //        auto mesh = std::make_shared<Mesh>(data);
    //        m_model = std::make_unique<Model>(mesh, material);
    //    }

    //public:
    //    void Draw()
    //    {
    //        m_model->SetTransform(m_Transform);
    //        m_model->Draw();
    //    }

    //    void Dispose()
    //    {
    //        m_model->Dispose();
    //    }

    //    static std::shared_ptr<Grid> Create(const int& slices, const float& gridSize, const glm::vec3& color)
    //    {
    //        auto ptr = std::shared_ptr<Grid>(new Grid(slices, gridSize, color));
    //        Gizmos::GetInstance().Add(ptr);
    //        return ptr;
    //    }
    //};

    ////-----------------------------------------------------------------------------
    ////
    ////-----------------------------------------------------------------------------
    //class Line : public BaseGizmo
    //{
    //protected:
    //    std::unique_ptr<Model> m_model;

    //    Line(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
    //    {
    //        std::vector<int> indices =
    //        {
    //            0, 1
    //        };

    //        std::vector<float> vertices =
    //        {
    //            start.x, start.y, start.z,
    //            end.x, end.y, end.z,
    //        };

    //        MeshData data(vertices, indices, {}, GL_LINES);
    //        auto material = Material::Get("gizmos");
    //        auto& shader = material->GetShader();
    //        shader.Bind();
    //        shader.SetVec3("color", color);
    //        shader.Unbind();

    //        auto mesh = std::make_shared<Mesh>(data);
    //        m_model = std::make_unique<Model>(mesh, material);
    //    }

    //public:
    //    void Draw()
    //    {
    //        m_model->SetTransform(m_Transform);
    //        m_model->Draw();
    //    }

    //    void Dispose()
    //    {
    //        m_model->Dispose();
    //    }

    //    static std::shared_ptr<Line> Create(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
    //    {
    //        auto ptr = std::shared_ptr<Line>(new Line(start, end, color));
    //        Gizmos::GetInstance().Add(ptr);
    //        return ptr;
    //    }
    //};

    ////-----------------------------------------------------------------------------
    ////
    ////-----------------------------------------------------------------------------
    //class WorldCoords : public BaseGizmo
    //{
    //protected:
    //    std::shared_ptr<Line> m_lineX;
    //    std::shared_ptr<Line> m_lineY;
    //    std::shared_ptr<Line> m_lineZ;

    //    WorldCoords(const Transform& transform)
    //    {
    //        const glm::vec3& p = transform.Position;

    //        m_lineX = Line::Create(p, p + glm::vec3(1, 0, 0), glm::vec3(1, 0, 0)); // Red
    //        m_lineY = Line::Create(p, p + glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)); // Green
    //        m_lineZ = Line::Create(p, p + glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)); // Blue
    //    }

    //public:
    //    void Dispose() override
    //    {
    //        if (m_lineX) m_lineX->Dispose();
    //        if (m_lineY) m_lineY->Dispose();
    //        if (m_lineZ) m_lineZ->Dispose();
    //    }

    //    static std::shared_ptr<WorldCoords> Create(const Transform& transform)
    //    {
    //        auto ptr = std::shared_ptr<WorldCoords>(new WorldCoords(transform));
    //        Gizmos::GetInstance().Add(ptr);
    //        return ptr;
    //    }
    //};

    ////-----------------------------------------------------------------------------
    ////
    ////-----------------------------------------------------------------------------
    //class Box : public BaseGizmo
    //{
    //protected:
    //    std::unique_ptr<Model> m_model;

    //    Box(const glm::vec3& min, const glm::vec3& max, const glm::vec3& color)
    //    {
    //        std::vector<glm::vec3> corners = {
    //            {min.x, min.y, min.z}, {max.x, min.y, min.z},
    //            {max.x, max.y, min.z}, {min.x, max.y, min.z},
    //            {min.x, min.y, max.z}, {max.x, min.y, max.z},
    //            {max.x, max.y, max.z}, {min.x, max.y, max.z}
    //        };

    //        // Indices for 12 edges of a box (wireframe)
    //        std::vector<int> indices = {
    //            0, 1, 1, 2, 2, 3, 3, 0, // bottom face
    //            4, 5, 5, 6, 6, 7, 7, 4, // top face
    //            0, 4, 1, 5, 2, 6, 3, 7  // vertical edges
    //        };

    //        std::vector<float> vertices;
    //        for (const auto& v : corners)
    //        {
    //            vertices.push_back(v.x);
    //            vertices.push_back(v.y);
    //            vertices.push_back(v.z);
    //        }

    //        MeshData data(vertices, indices, {}, GL_LINES);
    //        auto material = Material::Get("gizmos");
    //        auto& shader = material->GetShader();
    //        shader.Bind();
    //        shader.SetVec3("color", color);
    //        shader.Unbind();

    //        auto mesh = std::make_shared<Mesh>(data);
    //        m_model = std::make_unique<Model>(mesh, material);
    //    }

    //public:
    //    void Draw()
    //    {
    //        m_model->SetTransform(m_Transform);
    //        m_model->Draw();
    //    }

    //    void Dispose()
    //    {
    //        m_model->Dispose();
    //    }

    //    static std::shared_ptr<Box> Create(const glm::vec3& min, const glm::vec3& max, const glm::vec3& color)
    //    {
    //        auto ptr = std::shared_ptr<Box>(new Box(min, max, color));
    //        Gizmos::GetInstance().Add(ptr);
    //        return ptr;
    //    }
    //};
}
#pragma once

#include <vector>
#include <memory>

#include "singleton.hpp"
#include "transformable.hpp"
#include "model.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class BaseGizmo : public Transformable, public std::enable_shared_from_this<BaseGizmo>
{
public:
	virtual void Draw() {}
	virtual void Dispose() {}

    ~BaseGizmo() { Dispose(); }
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Gizmos : public Singleton<Gizmos>
{
protected:
    std::vector<std::shared_ptr<BaseGizmo>> m_all;

public:
	void Draw()
	{
        for (const auto& gizmo : m_all)
            if (gizmo)
                gizmo->Draw();
	}

    void Add(std::shared_ptr<BaseGizmo> gizmo)
    {
        if (gizmo)
            m_all.push_back(gizmo);
    }
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Grid : public BaseGizmo
{
protected:
    std::unique_ptr<Model> m_model;

    Grid(const int& slices, const float& gridSize, const glm::vec3& color)
    {
        int vertexCount = (slices + 1) * (slices + 1);
        std::vector<float> vertices(vertexCount * 3);

        int indexCount = slices * slices * 8;
        std::vector<int> indices(indexCount);

        int vi = 0;
        for (int j = 0; j <= slices; ++j)
        {
            for (int i = 0; i <= slices; ++i)
            {
                float x = static_cast<float>(i) / slices * gridSize;
                float y = 0.0f;
                float z = static_cast<float>(j) / slices * gridSize;

                vertices[vi++] = x;
                vertices[vi++] = y;
                vertices[vi++] = z;
            }
        }

        int ii = 0;
        for (int j = 0; j < slices; ++j)
        {
            for (int i = 0; i < slices; ++i)
            {
                int row1 = j * (slices + 1);
                int row2 = (j + 1) * (slices + 1);

                indices[ii++] = row1 + i;
                indices[ii++] = row1 + i + 1;

                indices[ii++] = row1 + i + 1;
                indices[ii++] = row2 + i + 1;

                indices[ii++] = row2 + i + 1;
                indices[ii++] = row2 + i;

                indices[ii++] = row2 + i;
                indices[ii++] = row1 + i;
            }
        }

        MeshData data(vertices, indices, {}, GL_LINES);
        auto material = Material::Get("gizmos");
        auto& shader = material->GetShader();
        shader.Bind();
        shader.SetVec3("color", color);
        shader.Unbind();

        auto mesh = std::make_shared<Mesh>(data);
        m_model = std::make_unique<Model>(mesh, material);
    }

public:
	void Draw()
	{
		m_model->SetTransform(m_Transform);
		m_model->Draw();
	}

	void Dispose()
	{
		m_model->Dispose();
	}

    static std::shared_ptr<Grid> Create(const int& slices, const float& gridSize, const glm::vec3& color)
    {
        auto ptr = std::shared_ptr<Grid>(new Grid(slices, gridSize, color));
        Gizmos::GetInstance().Add(ptr);
        return ptr;
    }
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Line : public BaseGizmo
{
protected:
    std::unique_ptr<Model> m_model;

    Line(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
    {
        std::vector<int> indices = 
        {
            0, 1
        };

        std::vector<float> vertices =
        {
            start.x, start.y, start.z,
            end.x, end.y, end.z,
        };

        MeshData data(vertices, indices, {}, GL_LINES);
        auto material = Material::Get("gizmos");
        auto& shader = material->GetShader();
        shader.Bind();
        shader.SetVec3("color", color);
        shader.Unbind();

        auto mesh = std::make_shared<Mesh>(data);
        m_model = std::make_unique<Model>(mesh, material);
    }

public:
    void Draw()
    {
        m_model->SetTransform(m_Transform);
        m_model->Draw();
    }

    void Dispose()
    {
        m_model->Dispose();
    }

    static std::shared_ptr<Line> Create(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
    {
        auto ptr = std::shared_ptr<Line>(new Line(start, end, color));
        Gizmos::GetInstance().Add(ptr);
        return ptr;
    }
};
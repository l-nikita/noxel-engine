#pragma once

#include <vector>

#include "singleton.hpp"
#include "transformable.hpp"
#include "model.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class BaseGizmo : public Transformable
{
public:
	virtual void Draw() {}
	virtual void Dispose() {}
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Gizmos : public Singleton<Gizmos>
{
protected:
	std::vector<BaseGizmo*> m_all;

public:
	void Draw()
	{
		for (auto gizmo : m_all)
		{
            if (gizmo)
                gizmo->Draw();
		}
	}

    void Add(BaseGizmo* gizmo)
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

public:
    Grid(int slices, float gridSize, const glm::vec4& color)
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
        for (uint32_t j = 0; j < slices; ++j)
        {
            for (uint32_t i = 0; i < slices; ++i)
            {
                uint32_t row1 = j * (slices + 1);
                uint32_t row2 = (j + 1) * (slices + 1);

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

        MeshData data(vertices, indices, {});
        auto material = Material::Get("gizmos");
        auto mesh = std::make_shared<Mesh>(data);
        m_model = std::make_unique<Model>(mesh, material);

        Gizmos::GetInstance().Add(this);
    }

	void Draw()
	{
		m_model->SetTransform(m_Transform);
		m_model->Draw();
	}

	void Dispose()
	{
		m_model->Dispose();
	}
};
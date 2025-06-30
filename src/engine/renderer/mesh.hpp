#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <optional>
#include <stdexcept>

#include "iengine.hpp"
#include "renderer.hpp"
#include "mesh_data.hpp"
#include "ebo.hpp"
#include "vao.hpp"
#include "vbo.hpp"

class Mesh
{
public:
	Mesh(MeshData data) : 
		m_data(std::move(data)),
		m_indicesCount(m_data.Indices.size()),
		m_EBO(m_data.Indices),
		m_VBO(m_data.Vertices)
	{
		if (m_data.Vertices.empty() || m_data.Indices.empty())
			throw std::runtime_error("Vertices or Indices are empty!");

		m_VAO.Bind();
		m_EBO.Bind();

		m_VAO.LinkToVAO(0, 3, 3, m_VBO);
		if (!m_data.UV.empty())
		{
			m_uvVBO = VBO(m_data.UV);
			m_VAO.LinkToVAO(1, 2, 2, *m_uvVBO);
		}

		m_VAO.Unbind();
		m_VBO.Unbind();
		m_EBO.Unbind();
	}

	~Mesh() { Dispose(); }

	void Draw() const
	{
		m_VAO.Bind();
		glDrawElements(m_data.PrimitiveType, m_indicesCount, GL_UNSIGNED_INT, 0);
		m_VAO.Unbind();
#ifdef _DEBUG
		g_Engine->GetRenderer()->AddDrawCallCount();
#endif
	}

	void Dispose()
	{
		m_VAO.Dispose();
		m_VBO.Dispose();
		if(m_uvVBO.has_value())
			m_uvVBO->Dispose();
		m_EBO.Dispose();
	}

	virtual glm::mat4 GetTransformMatrix() { return glm::mat4(1.0f); }

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&&) noexcept = default;
	Mesh& operator=(Mesh&&) noexcept = default;

protected:
	MeshData m_data;
	int m_indicesCount;

	EBO m_EBO;
	VAO m_VAO;
	VBO m_VBO;
	std::optional<VBO> m_uvVBO;
};
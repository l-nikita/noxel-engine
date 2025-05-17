#pragma once

#include <glm/glm.hpp>
#include <vector>

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
		m_VBO(m_data.Vertices),
		m_uvVBO(m_data.UV)
	{
		m_VAO.Bind();
		m_EBO.Bind();

		m_VAO.LinkToVAO(0, 3, 3, m_VBO);
		m_VAO.LinkToVAO(1, 2, 2, m_uvVBO);

		m_VAO.Unbind();
		m_VBO.Unbind();
		m_EBO.Unbind();
	}

	~Mesh() { Dispose(); }

	void Draw() const
	{
		m_VAO.Bind();
		glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
	}

	void Dispose() const
	{
		m_VAO.Dispose();
		m_VBO.Dispose();
		m_uvVBO.Dispose();
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
	VBO m_uvVBO;
};
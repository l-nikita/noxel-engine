#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ebo.hpp"
#include "vao.hpp"
#include "vbo.hpp"

class Mesh
{
public:
	Mesh(const std::vector<float>& vertices, const std::vector<int>& indices)
		: m_VAO(), m_VBO(vertices), m_EBO(indices)
	{
		m_indicesCount = indices.size();

		m_VAO.Bind();
		m_EBO.Bind();
		m_VAO.LinkToVAO(0, 3, 3, m_VBO);

		m_VAO.Unbind();
		m_VBO.Unbind();
		m_EBO.Unbind();
	}

	void Draw() const
	{
		m_VAO.Bind();
		glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
		m_VAO.Unbind();
	}

	void Dispose() const
	{
		m_VAO.Dispose();
		m_VBO.Dispose();
		m_EBO.Dispose();
	}

	EBO m_EBO;
	VAO m_VAO;
	VBO m_VBO;

	glm::mat4 GetTransformMatrix() { return glm::mat4(1.0f); }

protected:
	int m_indicesCount;
};
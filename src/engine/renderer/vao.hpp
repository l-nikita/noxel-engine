#pragma once

#include <glad/glad.h>
#include <vector>
#include "vbo.hpp"

class VAO
{
public:
	VAO()
	{
		glGenVertexArrays(1, &m_id);
		Bind();
	}

	void LinkToVAO(const int& location, const int& size, const int& vertexSize, const VBO& vbo)
	{
		Bind();
		vbo.Bind();

		glVertexAttribPointer(location, size, GL_FLOAT, false, vertexSize * sizeof(float), (void*)0);
		glEnableVertexAttribArray(location);
	}

	void Bind() const { glBindVertexArray(m_id); }
	void Unbind() const { glBindVertexArray(0); }
	void Dispose() const { glDeleteVertexArrays(1, &m_id); }

private:
	GLuint m_id;
};
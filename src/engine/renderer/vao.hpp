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

	~VAO() { Dispose(); }

	void LinkToVAO(const int& location, const int& size, const int& vertexSize, const VBO& vbo)
	{
		Bind();
		vbo.Bind();

		glVertexAttribPointer(location, size, GL_FLOAT, false, vertexSize * sizeof(float), (void*)0);
		glEnableVertexAttribArray(location);
	}

	void Bind() const { glBindVertexArray(m_id); }
	void Unbind() const { glBindVertexArray(0); }
	void Dispose() 
	{ 
		if (m_id != 0)
		{
			glDeleteVertexArrays(1, &m_id);
			m_id = 0;
		}
	}

	// ban copy, only move
	VAO(const VAO&) = delete;
	VAO& operator=(const VAO&) = delete;

	VAO(VAO&& other) noexcept : m_id(other.m_id) 
	{
		other.m_id = 0;
	}

	VAO& operator=(VAO&& other) noexcept 
	{
		if (this != &other) 
		{
			Dispose();
			m_id = other.m_id;
			other.m_id = 0;
		}
		return *this;
	}

private:
	GLuint m_id;
};
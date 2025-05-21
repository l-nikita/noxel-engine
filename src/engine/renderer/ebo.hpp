#pragma once

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	EBO(const std::vector<int>& indices)
	{
		glGenBuffers(1, &m_id);
		Bind();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
	}

	~EBO() { Dispose(); }

	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id); }
	void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	void Dispose() 
	{ 
		if (m_id != 0)
		{
			glDeleteBuffers(1, &m_id);
			m_id = 0;
		}
	}

	// ban copy, only move
	EBO(const EBO&) = delete;
	EBO& operator=(const EBO&) = delete;

	EBO(EBO&& other) noexcept : m_id(other.m_id)
	{
		other.m_id = 0;
	}

	EBO& operator=(EBO&& other) noexcept
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
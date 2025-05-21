#pragma once

#include <glad/glad.h>
#include <vector>

class VBO
{
public:
	VBO(const std::vector<float>& vertices)
	{
		glGenBuffers(1, &m_id);
		Bind();

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	}

	~VBO() { Dispose(); }

	void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_id); }
	void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void Dispose() 
	{
		if (m_id != 0)
		{
			glDeleteBuffers(1, &m_id);
			m_id = 0;
		}
	}

	// ban copy, only move
	VBO(const VBO&) = delete;
	VBO& operator=(const VBO&) = delete;

	VBO(VBO&& other) noexcept : m_id(other.m_id)
	{
		other.m_id = 0;
	}

	VBO& operator=(VBO&& other) noexcept
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
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
	void Dispose() const { glDeleteBuffers(1, &m_id); }

private:
	GLuint m_id;
};
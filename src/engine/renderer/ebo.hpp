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
	void Dispose() const { glDeleteBuffers(1, &m_id); }

private:
	GLuint m_id;
};
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "cmd.hpp"
#include "assets.hpp"

static GLuint CompileShader(GLenum type, const char* src)
{
    auto shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        char* infoLog = new char[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, infoLog);
        CMD_LOG("Shader Compilation Failed:" + std::string(infoLog), ERROR);

        delete[] infoLog;

        return 0;
    }

    return shader;
}

static bool CheckShaderLink(const GLuint& id)
{
    GLint linkStatus = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
    {
        GLint logLength = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(logLength);
        glGetProgramInfoLog(id, logLength, nullptr, infoLog.data());
        CMD_LOG("Shader Program Linking Failed:\n" + std::string(infoLog.data()), ERROR);

        glDeleteProgram(id);
        return false;
    }

    return true;
}

class ShaderProgram
{
public:
	ShaderProgram(const ShaderAsset* asset)
	{
        if (asset == nullptr)
        {
            CMD_LOG("ShaderAsset is nullptr!", ERROR);
            return;
        }

		m_id = glCreateProgram();

        auto vertShader = CompileShader(GL_VERTEX_SHADER, asset->GetShaderSource(GL_VERTEX_SHADER).c_str());
        auto fragShader = CompileShader(GL_FRAGMENT_SHADER, asset->GetShaderSource(GL_FRAGMENT_SHADER).c_str());
        glAttachShader(m_id, vertShader);
        glAttachShader(m_id, fragShader);

        glLinkProgram(m_id);

        if (!CheckShaderLink(m_id))
            return;

        glDetachShader(m_id, vertShader);
        glDetachShader(m_id, fragShader);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
	}

    void SetMatrix4(const std::string& name, const glm::mat4& matrix4) const
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, false, glm::value_ptr(matrix4));
    }

    GLint GetUniformLocation(const std::string& name) const
    {
        auto it = m_uniformLocations.find(name);
        if (it != m_uniformLocations.end())
            return it->second;

        GLint location = glGetUniformLocation(m_id, name.c_str());
        if (location == -1)
            CMD_LOG("Uniform [" + name + "] not found on shader.", ERROR);

        m_uniformLocations.try_emplace(name, location);
        return location;
    }

    void Bind() const { glUseProgram(m_id); }
    void Unbind() const { glUseProgram(0); }
    void Dispose() const { glDeleteProgram(m_id); }

    bool IsValid() const { return glIsProgram(m_id) == GL_TRUE; }

private:
	GLuint m_id;
    mutable std::unordered_map<std::string, GLint> m_uniformLocations;
};
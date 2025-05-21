#pragma once

#include <glad/glad.h>
#include <string>

#include "cmd.hpp"

class Texture 
{
public:
    Texture(TextureAsset* asset)
    {
        if (asset == nullptr)
        {
            CMD_LOG("TextureAsset is nullptr!", ERROR);
            return;
        }

        glGenTextures(1, &m_id);

        Bind();

        m_data = std::make_unique<TextureData>(asset->Load());
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            m_data->Format,
            m_data->Width,
            m_data->Height,
            0,
            m_data->Format,
            GL_UNSIGNED_BYTE,
            m_data->Image.data()
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);

        Unbind();
    }

    ~Texture() { Dispose(); }

    void SetMagFilter(GLenum filter) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter); }    
    void SetMinFilter(GLenum filter) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter); }

    void Bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }
    void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
    void Dispose() 
    { 
        if (m_id != 0)
        {
            glDeleteTextures(1, &m_id);
            m_id = 0;
        }
    }

    // ban copy, only move
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept
        : m_id(other.m_id), m_data(std::move(other.m_data))
    {
        other.m_id = 0;
    }

    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            Dispose();
            m_id = other.m_id;
            m_data = std::move(other.m_data);
            other.m_id = 0;
        }
        return *this;
    }

private:
    GLuint m_id;
    std::unique_ptr<TextureData> m_data;
};
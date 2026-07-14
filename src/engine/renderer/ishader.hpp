#pragma once

#include <variant>
#include <string>
#include <glm/glm.hpp>
#include "itexture.hpp"

using ShaderVar = std::variant<
    int,
    float,
    bool,
    std::string,
    glm::mat4,
    glm::vec3,
    glm::vec4,
    ITexture*>;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IShader
{
public:
    virtual ~IShader() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Release() = 0;

    virtual void SetVar( const std::string& name, const ShaderVar& var ) = 0;
    virtual ShaderVar& GetVar( const std::string& name ) = 0;
};
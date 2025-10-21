#pragma once

#include <glm/glm.hpp>
#include <variant>
#include <string>
#include "itexture.hpp"

using MaterialValue = std::variant<int, float, bool, std::string, glm::mat4, glm::vec3, glm::vec4, ITexture*>;

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

    virtual void SetParameter(const std::string& name, const MaterialValue& value) = 0;
    virtual MaterialValue* GetParameterValue(const std::string& name) = 0;
};
#pragma once

#include <glm/glm.hpp>
#include <string>
#include "ishader.hpp"

enum ShaderType
{
    VERTEX,
    PIXEL,
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IShaderProgram
{
public:
    virtual ~IShaderProgram() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Release() = 0;

    virtual IShader* GetPSShader() = 0;
    virtual IShader* GetVSShader() = 0;

    virtual void SetPSVar(const std::string& name, const ShaderVar& var) = 0;
    virtual void SetVSVar(const std::string& name, const ShaderVar& var) = 0;
};
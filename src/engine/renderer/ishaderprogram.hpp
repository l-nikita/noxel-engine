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

    virtual void SetPSParameter(const std::string& name, const MaterialVar& var) = 0;
    virtual void SetVSParameter(const std::string& name, const MaterialVar& var) = 0;
};
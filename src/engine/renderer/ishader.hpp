#pragma once

enum ShaderType
{
    VERTEX,
    PIXEL,
};

class IShader 
{
public:
    virtual ~IShader() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Destroy() = 0;
};
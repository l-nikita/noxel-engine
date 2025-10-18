#pragma once

#include <d3d11.h>
#include <unordered_map>
#include "renderer/directx11/dx11shaderprogram.hpp"
#include "renderer/ishader.hpp"
#include "renderer/ishaderprogram.hpp"

class DX11Shader : public IShader
{
public:
    DX11Shader(IShaderProgram* program)
        : m_program(static_cast<DX11ShaderProgram*>(program)), m_device(m_program->GetDevice()), m_context(m_program->GetContext()), m_inputLayout(nullptr)
    {}

    virtual ShaderType GetType() = 0;

    virtual void Init() = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Release() = 0;

    void SetParameter(const std::string& name, const MaterialValue& value) override
    {
        m_params[name] = value;
    }

    MaterialValue* GetParameterValue(const std::string& name) override
    {
        auto it = m_params.find(name);
        if (it != m_params.end())
            return &it->second;

        return nullptr;
    }

protected:
    DX11ShaderProgram* m_program;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    ID3D11InputLayout* m_inputLayout;
    std::unordered_map<std::string, MaterialValue> m_params;
};
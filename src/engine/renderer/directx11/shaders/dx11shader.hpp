#pragma once

#include <d3d11.h>
#include <unordered_map>
#include "dbg/debug.hpp"
#include "renderer/directx11/dx11shaderprogram.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11Shader : public IShader
{
public:
    explicit DX11Shader( IShaderProgram* program )
        : m_program(static_cast<DX11ShaderProgram*>(program)), m_device(m_program->GetDevice()),
          m_context(m_program->GetContext())
    {
        // parameters, buffers, resources...
    }

    virtual ShaderType GetType() = 0;

    void Bind() override = 0;
    void Unbind() override = 0;
    void Release() override = 0;

    void SetVar( const std::string& name, const ShaderVar& var ) override
    {
        if ( !m_vars.contains(name) )
        {
            CMD_ERROR("Unknown shader variable [" + name + "].", CMD::RENDERING);
            return;
        }

        m_vars[ name ] = var;
    }

    ShaderVar& GetVar( const std::string& name ) override
    {
        if ( !m_vars.contains(name) )
            FATAL_ERROR("Unknown shader variable [" + name + "].", CMD::RENDERING)

        return m_vars[ name ];
    }

protected:
    DX11ShaderProgram* m_program;

    std::unordered_map<std::string, ShaderVar> m_vars;

protected:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
};

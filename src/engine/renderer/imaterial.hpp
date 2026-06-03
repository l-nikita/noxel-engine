#pragma once

#include <string>
#include "irenderresource.hpp"

class IShaderProgram;
class ITexture;

constexpr auto MATERIAL_TEXTURES_SLOTS = 8;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IMaterial : public IRenderResource
{
public:
    virtual ~IMaterial() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    // Set a texture to a slot (e.g. diffuse, normal map, etc.)
    virtual void SetTexture( std::string paramName, ITexture* texture = nullptr ) = 0;
    virtual void SetShader( IShaderProgram* shader ) = 0;

    virtual IShaderProgram* GetShader() const = 0;
    virtual ITexture* GetTexture( std::string paramName ) = 0;
};

extern IMaterial* g_ErrorMaterial;

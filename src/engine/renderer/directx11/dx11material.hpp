#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <d3d11.h>
#include "renderer/imaterial.hpp"
#include "renderer/ishader.hpp"
#include "renderer/itexture.hpp"

class MaterialAsset;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11Material : public IMaterial
{
public:
    DX11Material( ID3D11Device* device, ID3D11DeviceContext* context, MaterialAsset* asset );

    ~DX11Material()
    {
        Release();
    }

    void Bind() override;
    void Unbind() override;
    void Release() override;

    void SetShader( IShaderProgram* shader ) override;
    void SetTexture( std::string paramName, ITexture* texture = nullptr ) override;

    IShaderProgram* GetShader() const override;
    ITexture* GetTexture( std::string paramName ) override;

private:
    MaterialAsset* m_asset;

    IShaderProgram* m_shader = nullptr;
    std::unordered_map<std::string, ITexture*> m_textures;

private:
    ID3D11DeviceContext* m_context;
    ID3D11Device* m_device;
};
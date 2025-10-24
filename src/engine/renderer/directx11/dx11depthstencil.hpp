#pragma once

#include <d3d11.h>

#include "renderer/idepthstencil.hpp"
#include "renderer/irendertexture.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11DepthStencil : public IDepthStencil
{
public:
    DX11DepthStencil(ID3D11Device* device, ID3D11DeviceContext* context, IRenderTexture* rt);
    DX11DepthStencil(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
    ~DX11DepthStencil() { Release(); }

    void Clear();
    void Release();

    void* GetResource() { return m_dsv; }
    ITexture* GetTexture() { return m_texture; }

private:
    ID3D11DepthStencilView* m_dsv = nullptr;
    ITexture* m_texture = nullptr;

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
};
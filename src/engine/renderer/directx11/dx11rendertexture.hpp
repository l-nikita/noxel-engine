#pragma once

#include <d3d11.h>

#include "renderer/irendertexture.hpp"
#include "renderer/idepthstencil.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11RenderTexture : public IRenderTexture
{
public:
    DX11RenderTexture(ID3D11Device* device, ID3D11DeviceContext* context, int width, int, bool depth, TextureFormat format);
    DX11RenderTexture(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture, bool depth, TextureFormat format);
    ~DX11RenderTexture() { Release(); }

    void Clear(glm::vec4 color) override;
    void Bind() override;
    void Unbind() override;
    void Release() override;

    void* GetResource() override { return m_rtv; }
    IDepthStencil* GetDepthStencil() override { return m_depthStencil; }
    ITexture* GetTexture() override { return m_texture; }

private:
    ID3D11RenderTargetView* m_rtv = nullptr;
    IDepthStencil* m_depthStencil = nullptr;
    ITexture* m_texture = nullptr;

    bool m_fromFactory = false;

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
};
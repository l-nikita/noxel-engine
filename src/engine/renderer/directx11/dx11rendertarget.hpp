#pragma once

#include <d3d11.h>

#include "renderer/irendertarget.hpp"

class DX11RenderTarget : public IRenderTarget
{
public:
    DX11RenderTarget(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
    ~DX11RenderTarget() { Release(); }

    void Clear(glm::vec4 color);
    void Bind();
    void Release();

    ID3D11RenderTargetView* GetRTV() { return m_rtv; }
    ID3D11DepthStencilView* GetDSV() { return m_dsv; }
    ITexture* GetTexture() override { return m_texture; }

private:
    ID3D11RenderTargetView* m_rtv = nullptr;
    ID3D11DepthStencilView* m_dsv = nullptr;
    ITexture* m_texture = nullptr;

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
};
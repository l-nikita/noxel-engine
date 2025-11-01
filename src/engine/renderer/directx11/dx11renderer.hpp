#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <initguid.h>
#include <cstdint>
#include <vector>

#include "renderer/irenderer.hpp"
#include "renderer/irendertexture.hpp"
#include "renderer/iviewport.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DX11Renderer : public IRenderer
{
public:
	void Init() override;
	void Shutdown() override;

	void OnWindowResize(int width, int height) override;

	IRenderFactory* GetFactory() const { return m_factory; }
	RenderCamera* GetCamera() const { return m_camera; }
	void SetCamera(RenderCamera* camera) { m_camera = camera; }

	bool IsVSyncEnabled() override { return m_vsyncEnabled; }
	void SetVSync(bool value) override { m_vsyncEnabled = value; }

	void SetPrimitiveTopology(PrimitiveTopology topology) override;

	void CreateDevice();
	void CreateSwapChain();
	void Swap() override;

	IRenderTexture* GetBackBufferRT() override { return m_backBuffer; }
	void CopyToBackBuffer(IRenderTexture* rt) override;
	void SetRenderTextures(IRenderTexture* rts[], std::uint32_t num, IDepthStencil* depthStencil) override;
	void BindRenderTextures(IRenderTexture* rts[], std::uint32_t num) override;
	void SetViewport(IViewport* viewport) override;

	ID3D11Device* GetDevice() { return m_device; }
	ID3D11DeviceContext* GetContext() { return m_context; }

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IRenderTexture* m_backBuffer;

	IDXGISwapChain1* m_swapChain = nullptr;

	IRenderFactory* m_factory;
	RenderCamera* m_camera;

	bool m_vsyncEnabled = true;
};
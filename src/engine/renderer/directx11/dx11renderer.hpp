#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <initguid.h>
#include <cstdint>
#include <vector>

#include "renderer/irenderer.hpp"
#include "renderer/irendertarget.hpp"
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

	IRenderFactory* GetFactory() { return m_factory; }
	RenderCamera* GetCamera() { return m_camera; };

	bool IsVSyncEnabled() override { return m_vsyncEnabled; }
	void SetVSync(bool value) override { m_vsyncEnabled = value; }

	void SetPrimitiveTopology(PrimitiveTopology topology) override;

	void CreateDevice();
	void CreateSwapChain();
	void Swap() override;

	IRenderTarget* GetBackBufferRT() { return m_backBuffer; }
	void CopyToBackBuffer(IRenderTarget* rt);
	void SetViewport(IViewport* viewport);

	ID3D11Device* GetDevice() { return m_device; }
	ID3D11DeviceContext* GetContext() { return m_context; }

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IRenderTarget* m_backBuffer;

	IDXGISwapChain1* m_swapChain = nullptr;

	IRenderFactory* m_factory;
	RenderCamera* m_camera;

	bool m_vsyncEnabled = true;
};
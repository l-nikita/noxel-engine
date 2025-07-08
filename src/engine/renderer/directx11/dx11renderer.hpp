#pragma once

#include <d3d11.h>
#include <cstdint>
#include <vector>
#include <initguid.h>
#include "renderer/irenderer.hpp"
#include "renderer/model.hpp"
#include "renderer/directx11/dx11factory.hpp"

class DX11Renderer : public IRenderer
{
public:
	bool Init() override;

	void Render() override;
	void Shutdown() override;

	void OnWindowResize(int width, int height) override;

	IRenderFactory* GetFactory() { return m_factory; }
	RenderCamera* GetCamera() { return m_camera; };

	bool IsVSyncEnabled() override { return m_vsyncEnabled; }
	void SetVSync(bool value) override { m_vsyncEnabled = value; }

	void SetPrimitiveTopology(PrimitiveTopology topology) override;

#ifdef _DEBUG
	int m_DrawCallCount;
	void AddDrawCallCount() override { m_DrawCallCount++; }
	int GetDrawCallCount() override { return m_DrawCallCount; }
#endif

	ID3D11Device* GetDevice() { return m_device; }
	ID3D11DeviceContext* GetContext() { return m_context; }

	std::vector<Model*> m_Models;

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr; // Render target view

	ID3D11Texture2D* m_depthStencil = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;

	IRenderFactory* m_factory;
	RenderCamera* m_camera;

	bool m_vsyncEnabled = true;
	uint16_t m_width = 0;
	uint16_t m_height = 0;
};
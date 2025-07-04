#pragma once

#include "renderer/irenderer.hpp"

class D3D11Renderer : public IRenderer
{
public:
	bool Init() override;

	void Render() override;
	void Shutdown() override;

	void OnWindowResize(int width, int height) override;

	RenderCamera* GetCamera() { return m_camera; };

#ifdef _DEBUG
	int m_DrawCallCount;
	void AddDrawCallCount() override { m_DrawCallCount++; }
	int GetDrawCallCount() override { return m_DrawCallCount; }
#endif

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr; // Render target view

	RenderCamera* m_camera;
	uint16_t m_width = 0;
	uint16_t m_height = 0;
};
#pragma once

#include <cstdint>

enum GraphicsAPI
{
	OPENGL,
	DIRECTX11,
};

enum PrimitiveTopology 
{
	LINES,
	TRIANGLES,
};

class RenderCamera;
class IRenderFactory;
class IRenderTexture;
class IDepthStencil;
struct IViewport;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IRenderer
{
public:
	virtual ~IRenderer() = default;

	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Swap() = 0;
	virtual void OnWindowResize(int width, int height) = 0;

	virtual IRenderTexture* GetBackBufferRT() = 0;
	virtual void CopyToBackBuffer(IRenderTexture* rt) = 0;
	virtual void SetViewport(IViewport* viewport) = 0;
	virtual void SetRenderTextures(IRenderTexture* rts[], std::uint32_t num, IDepthStencil* depthStencil) = 0;
	virtual void BindRenderTextures(IRenderTexture* rts[], std::uint32_t num) = 0;

	virtual IRenderFactory* GetFactory() = 0;
	virtual RenderCamera* GetCamera() = 0;
	virtual void SetCamera(RenderCamera* camera) = 0;

	virtual bool IsVSyncEnabled() = 0;
	virtual void SetVSync(bool value) = 0;

	virtual void SetPrimitiveTopology(PrimitiveTopology topology) = 0;
};

extern IRenderer* g_Renderer;
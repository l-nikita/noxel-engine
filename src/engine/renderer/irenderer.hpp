#pragma once

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
class IRenderTarget;

class IRenderer
{
public:
	virtual ~IRenderer() = default;

	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Swap() = 0;
	virtual void OnWindowResize(int width, int height) = 0;

	virtual IRenderTarget* GetBackBufferRT() = 0;
	virtual void CopyToBackBuffer(IRenderTarget* rt) = 0;

	virtual IRenderFactory* GetFactory() = 0;
	virtual RenderCamera* GetCamera() = 0;

	virtual bool IsVSyncEnabled() = 0;
	virtual void SetVSync(bool value) = 0;

	virtual void SetPrimitiveTopology(PrimitiveTopology topology) = 0;
};

extern IRenderer* g_Renderer;
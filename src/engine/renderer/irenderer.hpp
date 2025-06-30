#pragma once

class RenderCamera;

class IRenderer
{
public:
	virtual ~IRenderer() = default;

	virtual bool Init() = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;
	virtual void OnWindowResize(int width, int height) = 0;

	virtual RenderCamera* GetCamera() = 0;

#ifdef _DEBUG
	virtual void AddDrawCallCount() = 0;
	virtual int GetDrawCallCount() = 0;
#endif
};
#pragma once

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IRenderPipeline
{
public:
	virtual ~IRenderPipeline() = default;

	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void OnWindowResize(int width, int height) = 0;

#ifdef _DEBUG
	virtual void AddDrawCallCount() = 0;
	virtual std::uint32_t GetDrawCallCount() = 0;
#endif
};

extern IRenderPipeline* g_RenderPipeline;
#pragma once

#include <cstdint>
#include <vector>
#include "irendertarget.hpp"
#include "irender_pipeline.hpp"

class Model;

class DeferredRenderPipeline : public IRenderPipeline
{
public:
	void Init() override;
	void Render() override;
	void Shutdown() override;

#ifdef _DEBUG
	void AddDrawCallCount() { m_DrawCallCount++; }
	std::uint32_t GetDrawCallCount() { return m_DrawCallCount; }
#endif

public:
#ifdef _DEBUG
	std::uint32_t m_DrawCallCount;
#endif

	std::vector<Model*> m_Models;

private:
	IRenderTarget* m_BackBufferRT;
	IRenderTarget* m_TestRT;
};
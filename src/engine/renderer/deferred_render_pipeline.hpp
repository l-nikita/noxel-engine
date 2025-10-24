#pragma once

#include <cstdint>
#include <vector>
#include "irendertexture.hpp"
#include "irender_pipeline.hpp"
#include "ishaderprogram.hpp"
#include "idepthstencil.hpp"

class Model;

struct GBuffer
{
	IDepthStencil* DepthStencil;
	IRenderTexture* Color;
	IRenderTexture* Normal;
	IRenderTexture* Position;
	IRenderTexture* Material;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DeferredRenderPipeline : public IRenderPipeline
{
public:
	void Init() override;
	void Render() override;
	void Shutdown() override;
	void OnWindowResize(int width, int height) override;

	void DoGeometryPass();
	void DoLightingPass();

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
	GBuffer m_GBuffer;
	IShaderProgram* m_lightingShader;

	IViewport* m_backBufferViewport;
};
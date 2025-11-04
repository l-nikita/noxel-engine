#pragma once

#include <cstdint>
#include <vector>
#include "irendertexture.hpp"
#include "irender_pipeline.hpp"
#include "ishaderprogram.hpp"
#include "idepthstencil.hpp"
#include "irenderfactory.hpp"

class Model;

struct GBuffer
{
	IDepthStencil* DepthStencil;
	IRenderTexture* Color;
	IRenderTexture* Normal;
	IRenderTexture* Position;

	void Release()
	{
		auto factory = g_Renderer->GetFactory();

		if (DepthStencil) 
		{ 
			factory->Release(DepthStencil);
			DepthStencil = nullptr;
		}

		if (Color) 
		{
			factory->Release(Color);
			Color = nullptr;
		}

		if (Normal) 
		{
			factory->Release(Normal);
			Normal = nullptr;
		}

		if (Position) 
		{
			factory->Release(Position);
			Position = nullptr;
		}
	}
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
	void DoShadowPass();
	void DoLightingPass();
	void DoSkyboxPass();

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
	void SetupGBuffer(int width, int height);
	void SetupShadows();
	void SetupSkybox();

private:
	IMesh* m_skyboxMesh = nullptr;
	IMaterial* m_skyboxMat = nullptr;

	GBuffer m_GBuffer;
	IDepthStencil* m_shadowMap = nullptr;
	IShaderProgram* m_shadowsShader = nullptr;
	IShaderProgram* m_lightingShader = nullptr;

	RenderCamera* m_mainCamera = nullptr;
	IViewport* m_backBufferViewport = nullptr;
};
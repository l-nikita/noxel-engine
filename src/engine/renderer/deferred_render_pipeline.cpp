#include "directx11/dx11renderer.hpp"
#include "directx11/dx11texture.hpp"
#include "deferred_render_pipeline.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "gizmos.hpp"
#include "model.hpp"
#include "rendercamera.hpp"
#include "lighting.hpp"

#include "gui/igui.hpp"
#include "engine/window.hpp"
#include "engine/iengine.hpp"
#include "assetsystem/materialasset.hpp"

ITexture* g_ErrorTexture = nullptr;
IMaterial* g_ErrorMaterial = nullptr;

static Model* g_TestModel;
static Model* g_TestModel2;

void CalculateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	// Обнуляем тангенты и битангеты
	for (auto& v : vertices)
	{
		v.Tangent = glm::vec3(0.0f);
		v.Bitangent = glm::vec3(0.0f);
	}

	// Проходим по каждому треугольнику
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Vertex& v0 = vertices[indices[i + 0]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.Position - v0.Position;
		glm::vec3 edge2 = v2.Position - v0.Position;

		glm::vec2 deltaUV1 = v1.UV - v0.UV;
		glm::vec2 deltaUV2 = v2.UV - v0.UV;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent;
		glm::vec3 bitangent;

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		// Усреднение для вершин
		v0.Tangent += tangent; v1.Tangent += tangent; v2.Tangent += tangent;
		v0.Bitangent += bitangent; v1.Bitangent += bitangent; v2.Bitangent += bitangent;
	}

	// Ортогонализация TBN
	for (auto& v : vertices)
	{
		v.Tangent = glm::normalize(v.Tangent - v.Normal * glm::dot(v.Normal, v.Tangent));
		v.Bitangent = glm::normalize(glm::cross(v.Normal, v.Tangent));
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DeferredRenderPipeline::Init()
{
	auto factory = g_Renderer->GetFactory();

	// TODO: Memory leak, make resource manager
	g_ErrorTexture = factory->CreateTexture(TextureAsset::GetError());
	g_ErrorMaterial = factory->CreateMaterial(MaterialAsset::GetError());

	//----------------------------------------------------
	std::vector<Vertex> cubeVertices
	{
		// Front (+Z)
		{{-1, -1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1, -1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1,  1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1,  1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Back (-Z)
		{{ 1, -1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{-1, -1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{-1,  1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{ 1,  1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Top (+Y)
		{{-1,  1,  1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1,  1,  1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1,  1, -1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1,  1, -1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Bottom (-Y)
		{{-1, -1, -1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1, -1, -1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1, -1,  1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1, -1,  1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Right (+X)
		{{ 1, -1,  1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1, -1, -1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1,  1, -1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{ 1,  1,  1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Left (-X)
		{{-1, -1, -1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{-1, -1,  1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{-1,  1,  1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1,  1, -1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},
	};

	std::vector<std::uint32_t> cubeIndices
	{
		// Front
		0, 1, 2,  2, 3, 0,
		// Back
		4, 5, 6,  6, 7, 4,
		// Top
		8, 9,10, 10,11, 8,
		// Bottom
		12,13,14, 14,15,12,
		// Right
		16,17,18, 18,19,16,
		// Left
		20,21,22, 22,23,20
	};

	CalculateTangents(cubeVertices, cubeIndices);

	//----------------------------------------------------
	// TODO: Memory leak, make resource manager
	auto* mat = factory->CreateMaterial("dirt");
	auto* mesh = factory->CreateMesh(MeshData{ cubeVertices, cubeIndices });

	/*
	for (int i = 0; i < 45; i++)
	{
		for (int j = 0; j < 45; j++)
		{
			if (mat && mesh)
			{
				auto model = new Model(mesh, mat);
				model->m_Transform.Position = glm::vec3(j*3, 0, i * 3);
				m_Models.push_back(model);
			}
		}
	}
	*/

	auto model = new Model(mesh, mat);
	model->m_Transform.Position = glm::vec3(0, 0, 5) + glm::vec3(0, -3, -10);
	m_Models.push_back(model);

	auto model2 = new Model(mesh, mat);
	model2->m_Transform.Position = glm::vec3(0, 0, 8) + glm::vec3(0, -3, 7);
	m_Models.push_back(model2);

	auto model3 = new Model(mesh, mat);
	model3->m_Transform.Position = glm::vec3(-5, 0, 8) + glm::vec3(0, -3, -10);
	m_Models.push_back(model3);

	g_TestModel = new Model(mesh, mat);
	g_TestModel->m_Transform.Scale = glm::vec3(0.5f);
	m_Models.push_back(g_TestModel);

	g_TestModel2 = new Model(mesh, mat);
	g_TestModel2->m_Transform.Position = glm::vec3(0, -5, 0);
	g_TestModel2->m_Transform.Scale = glm::vec3(20, 1, 20);
	g_TestModel2->m_CastShadows = false;
	m_Models.push_back(g_TestModel2);

	//----------------------------------------------------
	int width, height;
	g_Engine->GetWindow()->GetSize(&width, &height);

	m_backBufferVP = factory->CreateViewport(0, 0, width, height);
	m_mainCamera = new RenderCamera(width, height);
	m_mainCamera->SetFOV(95.0f);
	m_mainCamera->SetNear(0.1f);
	m_mainCamera->SetFar(100.0f);

	g_Renderer->SetCamera(m_mainCamera);
	g_Renderer->SetViewport(m_backBufferVP);

	SetupGBuffer(width, height);
	SetupShadows();
	SetupSkybox();

	m_shadowsShader = factory->CreateShaderProgram(ShaderAsset::Get("shadowspass"));
	m_lightingShader = factory->CreateShaderProgram(ShaderAsset::Get("lightingpass"));

	CMD_INFO("RenderPipeline initialized", CMD::RENDERING);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DeferredRenderPipeline::SetupGBuffer(int width, int height)
{
	auto factory = g_Renderer->GetFactory();

	m_GBuffer.DepthStencil = factory->CreateDepthStencil(width, height, false);
	m_GBuffer.Color = factory->CreateRenderTexture(width, height, false);
	m_GBuffer.Normal = factory->CreateRenderTexture(width, height, false);
	m_GBuffer.Position = factory->CreateRenderTexture(width, height, false);
}

void DeferredRenderPipeline::SetupSkybox()
{
	auto factory = g_Renderer->GetFactory();

	std::vector<Vertex> cubeVertices
	{
		// Front (+Z)
		{{-1, -1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1, -1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1,  1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1,  1,  1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Back (-Z)
		{{ 1, -1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{-1, -1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{-1,  1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{ 1,  1, -1}, {0, 0, -1}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Top (+Y)
		{{-1,  1,  1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1,  1,  1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1,  1, -1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1,  1, -1}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Bottom (-Y)
		{{-1, -1, -1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1, -1, -1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1, -1,  1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1, -1,  1}, {0, -1, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Right (+X)
		{{ 1, -1,  1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{ 1, -1, -1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{ 1,  1, -1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{ 1,  1,  1}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},

		// Left (-X)
		{{-1, -1, -1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,1}, {1,1,1,1}},
		{{-1, -1,  1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,1}, {1,1,1,1}},
		{{-1,  1,  1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1,0}, {1,1,1,1}},
		{{-1,  1, -1}, {-1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0,0}, {1,1,1,1}},
	};

	std::vector<std::uint32_t> cubeIndices
	{
		// Front
		0, 1, 2,  2, 3, 0,
		// Back
		4, 5, 6,  6, 7, 4,
		// Top
		8, 9,10, 10,11, 8,
		// Bottom
		12,13,14, 14,15,12,
		// Right
		16,17,18, 18,19,16,
		// Left
		20,21,22, 22,23,20
	};

	m_skyboxMat = factory->CreateMaterial("skybox");
	m_skyboxMesh = factory->CreateMesh(MeshData{ cubeVertices, cubeIndices });
}

void DeferredRenderPipeline::SetupShadows()
{
	auto factory = g_Renderer->GetFactory();
	m_shadowsMapsVP = factory->CreateViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DeferredRenderPipeline::DoGeometryPass()
{
	IRenderTexture* rts[3] = {
		m_GBuffer.Color,
		m_GBuffer.Normal,
		m_GBuffer.Position
	};

	// 0.1f, 0.2f, 0.3f, 1.0f
	auto color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (auto& rt : rts)
		rt->Clear(color);

	m_GBuffer.DepthStencil->Clear();

	//---------------------
	auto dxrenderer = static_cast<DX11Renderer*>(g_Renderer);
	auto context = dxrenderer->GetContext();

	ID3D11ShaderResourceView* nullSRVs[3] = { nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 3, nullSRVs);

	g_Renderer->SetRenderTextures(rts, 3, m_GBuffer.DepthStencil);

	for (auto model : m_Models)
		model->Draw();

	static float totalTime = 0.0f;
	totalTime += g_Engine->DeltaTime();

	g_TestModel->SetPosition(glm::vec3(
		sin(totalTime) * 5.0f,
		-3.0f,
		cos(totalTime) * 5.0f
	));

	glm::quat rotation = glm::angleAxis(totalTime * 3.0f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
	g_TestModel->SetRotation(rotation);
}

//-----------------------------------------------------------------------------
void DeferredRenderPipeline::DoLightingPass()
{
	IRenderTexture* rts[3] = {
		m_GBuffer.Color,
		m_GBuffer.Normal,
		m_GBuffer.Position
	};

	auto dxrenderer = static_cast<DX11Renderer*>(g_Renderer);
	auto context = dxrenderer->GetContext();

	// move to lighting shader
	g_Renderer->BindRenderTextures(rts, 3);
	m_lightingShader->Bind();

	g_Renderer->SetPrimitiveTopology(TRIANGLES);
	context->Draw(3, 0);

	m_lightingShader->Unbind();
}

//-----------------------------------------------------------------------------
void DeferredRenderPipeline::DoShadowPass()
{
	auto dxrenderer = static_cast<DX11Renderer*>(g_Renderer);
	auto context = dxrenderer->GetContext();

	auto& lights = LightManager::GetInstance().GetLights();
	for (int i = 0; i < lights.size(); i++)
	{
		auto light = lights[i];
		if (!light || (light && !light->IsShadowsEnabled()))
			continue;

		auto shadowMap = light->GetShadowMap();
		auto depth = shadowMap->GetDepth();
		int size = shadowMap->GetSize();

		m_shadowsMapsVP->Resize(size, size);
		g_Renderer->SetViewport(m_shadowsMapsVP);

		g_Renderer->SetRenderTextures(nullptr, 0, depth);
		depth->Clear();

		m_shadowsShader->SetVSVar("lightview", light->GetViewMatrix());
		m_shadowsShader->SetVSVar("lightproj", light->GetProjectionMatrix());

		// draw scene
		for (auto model : m_Models)
		{
			if (!model->m_CastShadows)
				continue;

			for (auto mesh : model->GetMeshes())
			{
				m_shadowsShader->SetVSVar("model", model->m_Transform.GetTransformMatrix());
				m_shadowsShader->Bind();
				mesh->Draw();
				m_shadowsShader->Unbind();
			}
		}
	}

	g_Renderer->SetRenderTextures(nullptr, 0, nullptr);
}

//-----------------------------------------------------------------------------
void DeferredRenderPipeline::DoSkyboxPass()
{
	auto backBuffer = g_Renderer->GetBackBufferRT();
	g_Renderer->SetRenderTextures(&backBuffer, 1, m_GBuffer.DepthStencil);

	g_Renderer->SetPrimitiveTopology(TRIANGLES);
	m_skyboxMat->Bind();
	m_skyboxMesh->Draw();
	m_skyboxMat->Unbind();
}

//-----------------------------------------------------------------------------
void DeferredRenderPipeline::Render()
{
	auto backBuffer = g_Renderer->GetBackBufferRT();
	backBuffer->Clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	DoGeometryPass();
	DoShadowPass();

	g_Renderer->SetViewport(m_backBufferVP);
	g_Renderer->SetRenderTextures(&backBuffer, 1, nullptr);
	DoLightingPass();

	//DoSkyboxPass();

	g_Gui->Render();

	g_Renderer->Swap();
#ifdef _DEBUG
	m_DrawCallCount = 0;
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DeferredRenderPipeline::Shutdown()
{
	for (auto model : m_Models)
		delete model;

	m_Models.clear();
	m_GBuffer.Release();

	if (m_mainCamera)
	{
		delete m_mainCamera;
		m_mainCamera = nullptr;
	}

	if (m_backBufferVP)
	{
		delete m_backBufferVP;
		m_backBufferVP = nullptr;
	}

	if (m_shadowsMapsVP)
	{
		delete m_shadowsMapsVP;
		m_shadowsMapsVP = nullptr;
	}

	auto factory = g_Renderer->GetFactory();

	if (m_shadowsShader)
	{
		factory->Release(m_shadowsShader);
		m_shadowsShader = nullptr;
	}

	if (m_lightingShader)
	{
		factory->Release(m_lightingShader);
		m_lightingShader = nullptr;
	}

	if (m_skyboxMat)
	{
		factory->Release(m_skyboxMat);
		m_skyboxMat = nullptr;
	}

	if (m_skyboxMesh)
	{
		factory->Release(m_skyboxMesh);
		m_skyboxMesh = nullptr;
	}

	g_Renderer->Shutdown();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DeferredRenderPipeline::OnWindowResize(int width, int height)
{
	m_GBuffer.Release();

	g_Renderer->OnWindowResize(width, height);

	m_backBufferVP->Resize(width, height);
	g_Renderer->SetViewport(m_backBufferVP);

	SetupGBuffer(width, height);
	m_mainCamera->OnFrameBufferResize(width, height);
}
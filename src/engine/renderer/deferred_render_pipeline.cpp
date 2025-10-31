#include "directx11/dx11renderer.hpp"
#include "directx11/dx11texture.hpp"
#include "deferred_render_pipeline.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "gizmos.hpp"
#include "model.hpp"
#include "render_camera.hpp"

#include "gui/igui.hpp"
#include "engine/window.hpp"
#include "engine/iengine.hpp"
#include "assetsystem/materialasset.hpp"

ITexture* g_ErrorTexture = nullptr;
IMaterial* g_ErrorMaterial = nullptr;

static RenderCamera* g_Cam1;
static RenderCamera* g_Cam2;
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
	model->m_Transform.Position = glm::vec3(0, 0, 5);
	m_Models.push_back(model);

	auto model2 = new Model(mesh, mat);
	model2->m_Transform.Position = glm::vec3(0, 0, 8);
	m_Models.push_back(model2);

	auto model3 = new Model(mesh, mat);
	model3->m_Transform.Position = glm::vec3(0, 5, 8);
	m_Models.push_back(model3);

	g_TestModel = new Model(mesh, mat);

	g_TestModel2 = new Model(mesh, mat);
	g_TestModel2->m_Transform.Position = glm::vec3(0, -5, 0);
	g_TestModel2->m_Transform.Scale = glm::vec3(20, 1, 20);

	int width, height;
	g_Engine->GetWindow()->GetSize(&width, &height);

	m_backBufferViewport = factory->CreateViewport(0, 0, width, height);
	g_Cam1 = new RenderCamera(width, height);

	g_Renderer->SetCamera(g_Cam1);
	g_Renderer->SetViewport(m_backBufferViewport);

	m_GBuffer.DepthStencil = factory->CreateDepthStencil(width, height);
	m_GBuffer.Color = factory->CreateRenderTexture(width, height, false);
	m_GBuffer.Normal = factory->CreateRenderTexture(width, height, false);
	m_GBuffer.Position = factory->CreateRenderTexture(width, height, false);

	m_lightingShader = factory->CreateShaderProgram(ShaderAsset::Get("lightingpass"));

	CMD_INFO("RenderPipeline initialized", CMD::RENDERING);
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

	g_Renderer->SetRenderTextures(rts, 3, m_GBuffer.DepthStencil);

	for (auto model : m_Models)
		model->Draw();

	static float totalTime = 0.0f;
	totalTime += g_Engine->DeltaTime();

	//g_TestModel->m_Transform.Position = glm::vec3(
	//	sin(totalTime) * 5.0f,
	//	2.0f,
	//	cos(totalTime) * 5.0f
	//);

	g_TestModel->m_Transform.Position = glm::vec3(0, -3.0f, 0);

	g_TestModel->m_Transform.Scale = glm::vec3(0.5f);

	g_TestModel->Draw();
	g_TestModel2->Draw();
}

void DeferredRenderPipeline::DoLightingPass()
{
	IRenderTexture* rts[3] = {
		m_GBuffer.Color,
		m_GBuffer.Normal,
		m_GBuffer.Position
	};

	g_Renderer->BindRenderTextures(rts, 3);
	m_lightingShader->Bind();

	auto dxrenderer = static_cast<DX11Renderer*>(g_Renderer);
	auto context = dxrenderer->GetContext();

	g_Renderer->SetPrimitiveTopology(TRIANGLES);
	context->Draw(3, 0);

	m_lightingShader->Unbind();
}

void DeferredRenderPipeline::Render()
{
	auto backBuffer = g_Renderer->GetBackBufferRT();
	backBuffer->Clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	DoGeometryPass();

	backBuffer->Bind();
	DoLightingPass();

	/*
	auto dxrenderer = static_cast<DX11Renderer*>(g_Renderer);
	auto backBuffer = g_Renderer->GetBackBufferRT();

	//-------------------------------------------
	backBuffer->Clear(glm::vec4(0.1f, 0.2f, 0.3f, 1.0f));
	backBuffer->Bind();

	g_Renderer->SetViewport(m_backBufferViewport);
	g_Renderer->SetCamera(g_Cam1);

	for (auto model : m_Models)
	{
		model->Draw();
	}

	g_Cam2->DebugDraw();

	g_Gui->Render();

	//-------------------------------------------
	m_testRT->Clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	m_testRT->Bind();

	g_Renderer->SetViewport(m_testViewport);
	g_Renderer->SetCamera(g_Cam2);

	for (auto model : m_Models)
	{
		model->Draw();
	}

	static float angle2 = 0.0f;
	angle2 += g_Engine->DeltaTime() * 0.5f;

	glm::vec3 center = glm::vec3(0.0f, 0.0f, 5.0f);

	float radius = 5.0f;
	float camX = center.x + radius * cos(angle2);
	float camZ = center.z + radius * sin(angle2);
	float camY = center.y + 2.0f;

	g_Cam2->m_Transform.Position = glm::vec3(camX, camY, camZ);

	glm::vec3 direction = glm::normalize(g_Cam2->m_Transform.Position - center);
	g_Cam2->m_Transform.Rotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));

	g_Cam1->DebugDraw();

	//-------------------------------------------
	auto tex = static_cast<DX11Texture*>(m_testRT->GetTexture());
	auto backTex = static_cast<DX11Texture*>(backBuffer->GetTexture());

	dxrenderer->GetContext()->CopySubresourceRegion(
		backTex->GetResource(),
		0, 10, 25, 0,
		tex->GetResource(),
		0, nullptr);*/

		/*
		static float angle = 0.0f;
		static glm::vec3 pos = glm::vec3(0);

		angle += g_Engine->DeltaTime() * 1.0f;
		glm::quat rotation = glm::angleAxis(angle, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

		pos.y = 0.3f * glm::sin(g_Engine->ElapsedTime() * 1.0f);

		Gizmos::Line(pos, glm::vec3(0, 0, 5), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		glm::vec3 min(-1, -2, -1);
		glm::vec3 max(1, 2, 1);
		glm::vec3 center = (min + max) * 0.5f;

		glm::vec3 corners[8] = {
			{min.x, min.y, min.z},
			{min.x, min.y, max.z},
			{min.x, max.y, min.z},
			{min.x, max.y, max.z},
			{max.x, min.y, min.z},
			{max.x, min.y, max.z},
			{max.x, max.y, min.z},
			{max.x, max.y, max.z}
		};

		glm::vec4 color(1, 1, 0, 1);

		for (int i = 0; i < 8; i++)
		{
			glm::vec3 worldCenter = pos + (rotation * (center - center));
			glm::vec3 worldCorner = pos + (rotation * (corners[i] - center));
			Gizmos::Line(worldCenter, worldCorner, color);
		}

		Gizmos::Box(pos + (rotation * glm::vec3(3.0f, -1.0f, 0)), rotation, glm::vec3(-2, -0.5f, -0.5f), glm::vec3(2, 0.5f, 0.5f), glm::vec4(1, 1, 0, 0.1f));
		Gizmos::Box(pos, rotation, min, max, glm::vec4(1, 0, 1, 0.1f));

		//Gizmos::Triangle(
		//    glm::vec3(0, 0, 0),
		//    glm::vec3(0, 0, 0),
		//    glm::vec3(1, 0, 0),
		//    glm::vec3(0, 1, 0),
		//    glm::vec4(0, 0, 1, 1)
		//);
		*/

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
	{
		delete model;
		model = nullptr;
	}

	m_Models.clear();

	if (g_Cam1)
	{
		delete g_Cam1;
		g_Cam1 = nullptr;
	}

	g_Renderer->Shutdown();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DeferredRenderPipeline::OnWindowResize(int width, int height)
{
	g_Renderer->OnWindowResize(width, height);
	m_backBufferViewport->Resize(width, height);

	g_Cam1->OnFrameBufferResize(width, height);
}
#include "directx11/dx11renderer.hpp"
#include "directx11/dx11texture.hpp"
#include "deferred_render_pipeline.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "gizmos.hpp"
#include "model.hpp"

#include "gui/igui.hpp"
#include "engine/window.hpp"
#include "engine/iengine.hpp"
#include "assetsystem/assets.hpp"

ITexture* g_ErrorTexture = nullptr;
IMaterial* g_ErrorMaterial = nullptr;

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
        // Front
        {{-1, -1,  1}, {1,1,1,1}, {0,1}},
        {{ 1, -1,  1}, {1,1,1,1}, {1,1}},
        {{ 1,  1,  1}, {1,1,1,1}, {1,0}},
        {{-1,  1,  1}, {1,1,1,1}, {0,0}},

        // Back
        {{ 1, -1, -1}, {1,1,1,1}, {0,1}},
        {{-1, -1, -1}, {1,1,1,1}, {1,1}},
        {{-1,  1, -1}, {1,1,1,1}, {1,0}},
        {{ 1,  1, -1}, {1,1,1,1}, {0,0}},

        // Top
        {{-1,  1,  1}, {1,1,1,1}, {0,1}},
        {{ 1,  1,  1}, {1,1,1,1}, {1,1}},
        {{ 1,  1, -1}, {1,1,1,1}, {1,0}},
        {{-1,  1, -1}, {1,1,1,1}, {0,0}},

        // Bottom
        {{-1, -1, -1}, {1,1,1,1}, {0,1}},
        {{ 1, -1, -1}, {1,1,1,1}, {1,1}},
        {{ 1, -1,  1}, {1,1,1,1}, {1,0}},
        {{-1, -1,  1}, {1,1,1,1}, {0,0}},

        // Right
        {{ 1, -1,  1}, {1,1,1,1}, {0,1}},
        {{ 1, -1, -1}, {1,1,1,1}, {1,1}},
        {{ 1,  1, -1}, {1,1,1,1}, {1,0}},
        {{ 1,  1,  1}, {1,1,1,1}, {0,0}},

        // Left
        {{-1, -1, -1}, {1,1,1,1}, {0,1}},
        {{-1, -1,  1}, {1,1,1,1}, {1,1}},
        {{-1,  1,  1}, {1,1,1,1}, {1,0}},
        {{-1,  1, -1}, {1,1,1,1}, {0,0}},
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

    int width, height;
    g_Engine->GetWindow()->GetSize(&width, &height);

    m_backBufferViewport = factory->CreateViewport(0, 0, width, height);
    m_testViewport = factory->CreateViewport(0, 0, 512, 512);

    m_testRT = factory->CreateRenderTarget(512, 512);

    CMD_INFO("RenderPipeline initialized", CMD::RENDERING);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DeferredRenderPipeline::Render()
{
    auto dxrenderer = static_cast<DX11Renderer*>(g_Renderer);
    auto backBuffer = g_Renderer->GetBackBufferRT();

    //-------------------------------------------
    backBuffer->Clear(glm::vec4(0.1f, 0.2f, 0.3f, 1.0f));
    backBuffer->Bind();

    g_Renderer->SetViewport(m_backBufferViewport);

    for (auto model : m_Models)
    {
        model->Draw();
    }

    g_Gui->Render();

    //-------------------------------------------
    m_testRT->Clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    m_testRT->Bind();

    g_Renderer->SetViewport(m_testViewport);

    for (auto model : m_Models)
    {
        model->Draw();
    }

    auto tex = static_cast<DX11Texture*>(m_testRT->GetTexture());
    auto backTex = static_cast<DX11Texture*>(backBuffer->GetTexture());

    dxrenderer->GetContext()->CopySubresourceRegion(
        backTex->GetResource(),
        0, 10, 10, 0,
        tex->GetResource(),
        0, nullptr);

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

    if (m_testRT)
    {
        delete m_testRT;
        m_testRT = nullptr;
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
}